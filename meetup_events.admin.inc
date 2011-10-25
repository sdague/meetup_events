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

function meetup_events_admin_settings() {
  // Get an array of node types with internal names as keys and
  // "friendly names" as values. E.g.,
  // array('page' => 'Page', 'story' => 'Story')
  $options = node_get_types('names');
  $form['meetup_events_node_types'] = array(
    '#type' => 'checkboxes',
    '#title' => t('These types of nodes should be synced to meetup'),
    '#options' => $options,
    '#default_value' => variable_get('meetup_events_node_types', array()),
    '#description' => t('These types of content that should be synchronized'),
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
