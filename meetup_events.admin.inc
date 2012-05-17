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

/***
 * Form for main settings page
 */

function meetup_events_admin_settings() {
  // Get an array of node types with internal names as keys and
  // "friendly names" as values. E.g.,
  // array('page' => 'Page', 'story' => 'Story')
  ctools_include('dependent');

  $options = meetup_events_get_allowed_types();

  $values = variable_get("meetup_events", array());

  $form["meetup_events"] = array(
    '#type' => 'fieldset',
    '#tree' => TRUE,
    '#title' => 'Enabled types',
    '#description' => 'Select node types below to enable for meetup synchronization'
  );

  foreach ($options as $type => $name) {

    $form["meetup_events"][$type . "_enabled"] = array(
      '#type' => 'checkboxes',
      '#options' => array($type => $name),
      '#default_value' => $values[$type . "_enabled"],
      # '#description' => t('Only nodes of these types will be synchronized to meetup.
      #Because meetup requires dates, only node types that can support date fields
      #are eligible for syncing.')
    );

    $dates = meetup_events_get_date_fields_for_type($type);

    $form["meetup_events"][$type] = array(
      '#type' => 'fieldset',
      '#input' => TRUE,
      '#id' => 'group-' . $type,
      '#prefix' => '<div id="group-' . $type. '-wrapper">', # <div id="group-' . $type . '">',
      '#suffix' => '</div>', # </div>',
      '#process' => array('ctools_dependent_process','expand_checkboxes'),
      '#dependency' => array("edit-meetup-events-" . $type . "-enabled-$type" => array(TRUE))
    );

    $form["meetup_events"][$type]["date_field"] = array(
      '#title' => t("Date Field"),
      '#description' => 'Which date field should we use for the event date / time',
      '#type' => 'select',
      '#options' => meetup_events_get_date_fields_for_type($type),
      '#default_value' => $values[$type]["date_field"],
    );

    $form["meetup_events"][$type]["body"] = array(
      '#title' => t("Body Contents"),
      '#description' => 'The body content which will be synced to meetup (can use tokens if enabled)',
      '#type' => 'textarea',
      '#default_value' => ($values[$type]["body"]) ? $values[$type]["body"] : t("Come to our awesome meetup!")
    );

    if (module_exists('token')) {
      $form["meetup_events"][$type]['view']['token_help'] = array(
        '#title' => t('Replacement patterns'),
        '#type' => 'fieldset',
        '#collapsible' => TRUE,
        '#collapsed' => TRUE,
        //      '#description' => t('Prefer raw-text replacements for text to avoid problems with HTML entities!'),
      );

      $form["meetup_events"][$type]['view']['token_help']['help'] = array(
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

  /* $form['buttons']['save'] = array( */
  /*   '#type' => 'submit', */
  /*   '#value' => t('Save') */
  /* ); */

  return system_settings_form($form);
}

/* function meetup_events_admin_settings_submit($form_id, &$form_state) { */
/*   drupal_set_message("<pre>" . print_r($form_state, TRUE) . "</pre>"); */
/* } */