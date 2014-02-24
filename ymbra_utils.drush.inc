<?php

/**
 * Implements hook_drush_command().
 */
function ymbra_utils_drush_command() {
  $commands = array();

  $commands['ymbra-ct-info'] = array(
    'description' => 'View information about a content type.',
    'drupal_dependencies' => array('node'),
    'core' => array(7),
    'arguments' => array(
        'bundle' => 'Name of bundle of content type to delete.',
    ),
    'examples' => array(
        'ymbra-ct-info insect' => 'show cursory info about insect node type',
        'ct-info' => 'list all node types with counts of published and unpublished nodes',
    ),
    'callback' => 'ymbra_utils_ct_info',
    'options' => array(
      'pipe' => 'Return field information table as CSV.',
    )
  );

  return $commands;
}

/**
 * Display a list of content types with their fields.
 *
 * @param string $bundle
 *  Bundle name.
 *
 * @return array
 */
function ymbra_utils_ct_info($bundle = NULL) {
  $rows = array();
  if ($bundle) {
    $bundles = array($bundle);
  }
  else {
    $bundles = node_type_get_names();
  }

  $rows[] = array(
    dt('Content type'),
    dt('Content type label'),
    dt('Field name'),
    dt('Field label'),
    dt('Field type'),
    dt('Feature'),
  );
  $rows[] = array(
    dt('-----------------'),
    dt('-------------------'),
    dt('-----------------'),
    dt('-----------------'),
    dt('-----------------'),
    dt('-----------'),
  );

  $features = features_get_features();
  $features_fields = ymbra_utils_ct_info_features('field', $features);
  $features_ct = ymbra_utils_ct_info_features('node', $features);
  foreach ($bundles as $bundle_name => $bundle_label) {
    $row = array($bundle_name, $bundle_label, '', '', '', implode(';', $features_ct[$bundle_name]['features']));
    $rows[] = $row;
    $pipe[] = implode(',', $row);
    $fields = ymbra_utils_ct_info_fields('node', $bundle_name);
    foreach ($fields as $field) {
      $row = array('', '', $field['field_name'], $field['field_label'], $field['field_type'], implode(';', $features_fields['node-' . $bundle_name . '-' . $field['field_name']]['features']));
      $rows[] = $row;
      $pipe[] = implode(',', $row);
    }
  }

  drush_print_table($rows, TRUE);
  drush_print_pipe($pipe);
  return $rows;
}

/**
 * Wrapper to display the field information we want.
 *
 * @param string $entity_type
 *  Type of the entity.
 * @param string $bundle
 *  Bundle name.
 *
 * @return array
 */
function ymbra_utils_ct_info_fields($entity_type, $bundle) {
  $instances = field_info_instances($entity_type, $bundle);
  $fields = array();
  foreach ($instances as $name => $instance) {
      $field = field_info_field($instance['field_name']);
      $fields[$name]['field_name'] = $instance['field_name'];
      $fields[$name]['field_label'] =  check_plain($instance['label']);
      $fields[$name]['field_type'] = $field['type'];
  }

  return $fields;
}

/**
 * Gets elements out of the feature info.
 *
 * @param string $type
 *  Type of the items to retrieve, ex. "field" or "node".
 * @param array $features
 *  Array of features as returned from features_get_features().
 *
 * @see features_get_info()
 *
 * @return array
 */
function ymbra_utils_ct_info_features($type = 'field', $features = NULL) {
  if (empty($features)) {
    $features = features_get_features();
  }
  $items = array();
  foreach ($features as $name => $feature) {
    if ($elements = $feature->info['features'][$type]) {
      foreach ($elements as $element) {
        $items[$element]['features'][] = $name;
      }
    }
  }
  return $items;
}
