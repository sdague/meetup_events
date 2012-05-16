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

// ctools_include('dependent');

function meetup_events_get_date_fields_for_type($type) {
  $fields = array();
  $results = db_query("SELECT c.field_name, c.label from {content_node_field_instance} c INNER JOIN {node_type} n ON c.type_name=n.type where c.widget_module='date' and c.type_name='%s' ORDER BY n.name, c.label", $type);
  while($obj = db_fetch_object($results)) {
    $fields[$obj->field_name] = $obj->label;
  }
  return $fields;
}

function meetup_events_get_allowed_types() {
  $options = array();
  $results = db_query("SELECT c.type_name, n.name from {content_node_field_instance} c INNER JOIN {node_type} n ON c.type_name=n.type where c.widget_module='date' ORDER BY n.name, c.label");
  while($obj = db_fetch_object($results)) {
    $options[$obj->type_name] = $obj->name;
  }
  return $options;
}



function meetup_events_is_type_syncable($type) {
  if (variable_get("meetup_events_" . $type . "_enable", FALSE)) {
    return TRUE;
  } else {
    return FALSE;
  }
}

function meetup_events_admin_settings() {
  // Get an array of node types with internal names as keys and
  // "friendly names" as values. E.g.,
  // array('page' => 'Page', 'story' => 'Story')
  ctools_include('dependent');

  $options = meetup_events_get_allowed_types();

  /* $form["fields"] = array( */
  /*   '#type' => 'fieldset', */
  /*   '#title' => 'Enabled types', */
  /*   '#description' => 'Select node types below to enable for meetup synchronization' */
  /* ); */

  foreach ($options as $type => $name) {

    $form["fields"]["meetup_events_" . $type . "_enable"] = array(
      '#type' => 'checkboxes',
      '#options' => array($type => $name),
      # '#tree' => TRUE,
      '#default_value' => variable_get("meetup_events_" . $type . "_enable", array()),
      # '#description' => t('Only nodes of these types will be synchronized to meetup.
      #Because meetup requires dates, only node types that can support date fields
      #are eligible for syncing.')
    );

    $dates = meetup_events_get_date_fields_for_type($type);

    $form["fields"]["meetup_events_" . $type] = array(
      '#type' => 'fieldset',
      '#input' => TRUE,
      '#tree' => TRUE,
      '#id' => 'group-' . $type,
      '#prefix' => '<div id="group-' . $type. '-wrapper">', # <div id="group-' . $type . '">',
      '#suffix' => '</div>', # </div>',
      '#process' => array('ctools_dependent_process','expand_checkboxes'),
      '#dependency' => array("edit-meetup-events-$type-enable-$type" => array(TRUE))
    );

    $form["fields"]["meetup_events_" . $type]["date"] = array(
      '#title' => t("Date Field"),
      '#description' => 'Which date field should we use for the event date / time',
      '#type' => 'select',
      '#options' => meetup_events_get_date_fields_for_type($type),
      '#default_value' => variable_get("meetup_events_" . $type . "_date", $dates[0]),
    );

    $form["fields"]["meetup_events_" . $type]["body"] = array(
      '#title' => t("Body Contents"),
      '#description' => 'The body content which will be synced to meetup (can use tokens if enabled)',
      '#type' => 'textarea',
      '#default_value' => variable_get("meetup_events_" . $type . "_body", t("Come to our awesome meetup!")),
    );

    if (module_exists('token')) {
      $form["fields"]["meetup_events_" . $type]['view']['token_help'] = array(
        '#title' => t('Replacement patterns'),
        '#type' => 'fieldset',
        '#collapsible' => TRUE,
        '#collapsed' => TRUE,
        //      '#description' => t('Prefer raw-text replacements for text to avoid problems with HTML entities!'),
      );

      $form["fields"]["meetup_events_" . $type]['view']['token_help']['help'] = array(
        '#value' => theme('token_help', 'node'),
      );
    }
  }

  $form['api'] = array(
    '#type' => 'fieldset',
    '#title' => t('Meetup.com API information'),
  );

  $form['api']['meetup_events_meetup_key'] = array(
    '#type' => 'textfield',
    '#title' => t('Meetup API Key'),
    '#default_value' => variable_get('meetup_events_meetup_key', 0),
    '#description' => t('The meetup API key for your user'),
  );

  $form['api']['meetup_events_meetup_group'] = array(
    '#type' => 'textfield',
    '#title' => t('Meetup Group ID'),
    '#default_value' => variable_get('meetup_events_meetup_group', 0),
    '#description' => t('The meetup group id'),
  );

  return system_settings_form($form);
}
