<?php

/**
 * @file
 * Builds placeholder replacement tokens for price-n-stock-related data.
 */

use Drupal\Core\Entity\TypedData\EntityDataDefinitionInterface;
use Drupal\Core\Render\BubbleableMetadata;
use Drupal\Core\TypedData\DataReferenceDefinitionInterface;

/**
 * Implements hook_token_info_alter().
 */
function node_token_token_info_alter(&$data) {
  $types = \Drupal::entityTypeManager()
    ->getStorage('node_type')
    ->loadMultiple();

  /** @var \Drupal\Core\Entity\EntityFieldManagerInterface $entity_field_manager */
  $entity_field_manager = \Drupal::service('entity_field.manager');
  $node_fields = $entity_field_manager->getFieldStorageDefinitions('node');

  // Create dedicated token type for each node type.
  /** @var \Drupal\node\Entity\NodeType $type */
  foreach ($types as $bundle => $type) {
    $token_type_name = 'node-' . $bundle;

    $data['types'][$token_type_name] = $data['types']['node'];
    $data['types'][$token_type_name]['name'] = $type->label();
    $data['types'][$token_type_name]['needs-data'] = $token_type_name;
    if ($help = $type->getDescription()) {
      $data['types'][$token_type_name]['description'] = $help;
    }

    $bundle_fields = $entity_field_manager->getFieldDefinitions('node', $bundle);
    $fields_to_remove = array_diff_key($node_fields, $bundle_fields);
    $data['tokens'][$token_type_name] = $data['tokens']['node'];
    // Remove useless fields.
    $data['tokens'][$token_type_name] = array_diff_key($data['tokens'][$token_type_name], $fields_to_remove);

    $bundle_fields_definition = array_intersect_key($node_fields, $bundle_fields);
    /** @var \Drupal\Core\Field\FieldStorageDefinitionInterface $field */
    foreach ($bundle_fields_definition as $field_name => $field) {
      // Make sure a token type exists for this entity.
      $token_type = \Drupal::service('token.entity_mapper')->getTokenTypeForEntityType('node');
      if (empty($token_type)) {
        continue;
      }
      $field_token_name = $token_type . '-' . $field_name;
      $field_token_name_with_bundle = $token_type . '-' . $bundle . '-' . $field_name;
      // Make sure a token data exists for this field.
      if (!isset($data['tokens'][$field_token_name])) {
        continue;
      }

      // Prepare entity reference property tokens.
      foreach ($field->getPropertyDefinitions() as $property => $property_definition) {
        if (($property_definition instanceof DataReferenceDefinitionInterface) && ($property_definition->getTargetDefinition() instanceof EntityDataDefinitionInterface)) {
          /** @var \Drupal\Core\Entity\TypedData\EntityDataDefinitionInterface $target_definition */
          $target_definition = $property_definition->getTargetDefinition();
          $referenced_entity_type = $target_definition->getEntityTypeId();
          if ($referenced_entity_type !== 'node') {
            continue;
          }

          /** @var \Drupal\field\Entity\FieldConfig $field_of_bundle */
          $field_of_bundle = $bundle_fields[$field_name];
          $settings = $field_of_bundle->getSetting('handler_settings');
          $target_bundles = $settings['target_bundles'] ?? [];

          if (count($target_bundles) !== 1) {
            continue;
          }

          // Create new token type for field of bundle.
          reset($target_bundles);
          $target_bundle = key($target_bundles);
          if (isset($data['tokens'][$field_token_name_with_bundle])) {
            continue;
          }
          $data['types'][$field_token_name_with_bundle] = $data['types'][$field_token_name];
          $data['tokens'][$field_token_name_with_bundle] = $data['tokens'][$field_token_name];
          $data['tokens'][$field_token_name_with_bundle][$property]['type'] = 'node-' . $target_bundle;
          $data['tokens'][$token_type_name][$field->getName()]['type'] = $field_token_name_with_bundle;
        }
      }
    }
  }
}

/**
 * Implements hook_tokens().
 */
function node_token_tokens($type, $tokens, array $data, array $options, BubbleableMetadata $bubbleable_metadata) {
  if (!preg_match('/node-(.*)/i', $type, $matches)) {
    return [];
  }

  $bundle = $matches[1];
  $replacements = [];
  $types = \Drupal::entityTypeManager()
    ->getStorage('node_type')
    ->loadMultiple();

  if (isset($types[$bundle]) && $data[$type]) {
    // Replace custom token as node token.
    $data['node'] = $data[$type];
    $replacements = \Drupal::moduleHandler()->invokeAll(
      'tokens',
      ['node',
        $tokens,
        $data,
        $options,
        $bubbleable_metadata,
      ]
    );
  }

  return $replacements;
}
