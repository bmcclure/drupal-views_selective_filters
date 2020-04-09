<?php

/**
 * @file
 * View execution hooks for views_selective_filters.
 */

use Drupal\views\ViewExecutable;
use Drupal\views\Plugin\views\query\QueryPluginBase;

/**
 * Implements hook_views_query_alter().
 *
 * Since this filter plugin screws up entity reference fields, find any filter
 * submission that ends with _target_id_selective and fix query conditions.
 *
 * From: 'node__field_some_entity_reference.field_some_entity_reference'
 * To: 'node__field_some_entity_reference.field_some_entity_reference_target_id'
 */
function views_selective_filters_views_query_alter(ViewExecutable $view, QueryPluginBase $query) {
  $field_suffix = '_selective';
  $missing = '_target_id';
  $uses_selective_filters = FALSE;
  $replacement_map = [];
  foreach ($view->filter as $filter_name => $filter) {
    if (preg_match("/${missing}${field_suffix}$/", $filter_name) === 1) {
      $uses_selective_filters = TRUE;
      $replace_with = preg_replace("/${field_suffix}$/", '', $filter_name);
      $look_for = preg_replace("/${missing}$/", '', $replace_with);
      $replacement_map[$look_for] = $replace_with;
    }
  }

  if ($uses_selective_filters) {
    foreach ($query->where as &$condition_group) {
      foreach ($condition_group['conditions'] as &$condition) {
        $exploded = explode('.', $condition['field']);
        $search_key = array_pop($exploded);
        if (isset($replacement_map[$search_key])) {
          $condition['field'] = preg_replace("/.${search_key}$/", '.' . $replacement_map[$search_key], $condition['field']);
        }
      }
    }
  }
}
