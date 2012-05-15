<?php // -*- drupal -*-
  /**
   * @file
   * Administration page callbacks for the lending module.
   */

  /**
   * Form builder. Configure lending.
   *
   * @ingroup forms
   * @see system_settings_form().
   */

function meetup_events_get_allowed_types() {
  $options = array();
  $results = db_query("SELECT c.type_name, n.name from {content_node_field_instance} c INNER JOIN {node_type} n ON c.type_name=n.type where c.widget_module='date' ORDER BY n.name, c.label");
  while($obj = db_fetch_object($results)) {
    $options[$obj->type_name] = $obj->name;
  }
  return $options;
}

function meetup_events_admin_settings() {
  // Get an array of node types with internal names as keys and
  // "friendly names" as values. E.g.,
  // array('page' => 'Page', 'story' => 'Story')
  $options = meetup_events_get_allowed_types();

  $form['meetup_events_node_types'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Sync nodes of this type to meetup'),
    '#options' => $options,
    '#default_value' => variable_get('meetup_events_node_types', array()),
    '#description' => t('Only nodes of these types will be synchronized to meetup.
Because meetup requires dates, only node types that can support date fields
are eligible for syncing.')
  );

  $form['meetup_events_meetup_key'] = array(
    '#type' => 'textfield',
    '#title' => t('Meetup API Key'),
    '#default_value' => variable_get('meetup_events_meetup_key', 0),
    '#description' => t('The meetup API key for your user'),
  );

  $form['meetup_events_meetup_group'] = array(
    '#type' => 'textfield',
    '#title' => t('Meetup Group ID'),
    '#default_value' => variable_get('meetup_events_meetup_group', 0),
    '#description' => t('The meetup group id'),
  );

  return system_settings_form($form);
}
