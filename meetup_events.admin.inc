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

function meetup_events_get_numeric_fields_for_type($type) {
  $fields = array("none" => "none");
  # first get all number fields that are on the type
  $results = db_query("SELECT c.field_name, c.label from {content_node_field_instance} c INNER JOIN {node_type} n ON c.type_name=n.type where c.widget_module='number' and c.type_name='%s' ORDER BY n.name, c.label", $type);
  while($obj = db_fetch_object($results)) {
    $fields[$obj->field_name] = $obj->label;
  }

  # then get all number fields that are on the nodereferences
  $results = db_query("SELECT c.field_name, c.label, cnf.global_settings from {content_node_field_instance} c INNER JOIN {node_type} n ON c.type_name=n.type INNER JOIN {content_node_field} cnf ON c.field_name=cnf.field_name where c.widget_module='nodereference' and c.type_name='%s' ORDER BY n.name, c.label", $type);
  while($obj = db_fetch_object($results)) {
    $field = $obj->field_name;
    $label = $obj->label;
    $ref_types = unserialize($obj->global_settings);

    foreach ($ref_types["referenceable_types"] as $key => $value) {
      if ($value) {
        # and then see what numbers they have
        $ref_results = db_query("SELECT c.field_name, c.label from {content_node_field_instance} c INNER JOIN {node_type} n ON c.type_name=n.type where c.widget_module='number' and c.type_name='%s' ORDER BY n.name, c.label", $key);
        while($ref = db_fetch_object($ref_results)) {
          $fields[$field . ":" . $ref->field_name] = $label . " (node ref) => " . $ref->label;
        }
      }
    }
  }
  return $fields;
}

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

/***
 * Form for main settings page
 */

function meetup_events_admin_settings() {
  ctools_include('dependent');

  $options = meetup_events_get_allowed_types();

  $settings = variable_get("meetup_events", array());

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
      '#default_value' => $settings[$type . "_enabled"],
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
      '#default_value' => $settings[$type]["date_field"],
    );

    $form["meetup_events"][$type]["venue_field"] = array(
      '#title' => t("Venue Id Field"),
      '#description' => 'Which numeric field should we use for numeric venue id for meetup (can be on a referenced node)',
      '#type' => 'select',
      '#options' => meetup_events_get_numeric_fields_for_type($type),
      '#default_value' => $settings[$type]["venue_field"],
    );

    $form["meetup_events"][$type]["venue_lat"] = array(
      '#title' => t("Venue Latitude Field"),
      '#description' => 'Which numeric field should we use for Latitude to force the position of the map marker (optional)',
      '#type' => 'select',
      '#options' => meetup_events_get_numeric_fields_for_type($type),
      '#default_value' => $settings[$type]["venue_lat"],
    );

    $form["meetup_events"][$type]["venue_lon"] = array(
      '#title' => t("Venue Longitude Field"),
      '#description' => 'Which numeric field should we use for Longitude to force the position of the map marker (optional)',
      '#type' => 'select',
      '#options' => meetup_events_get_numeric_fields_for_type($type),
      '#default_value' => $settings[$type]["venue_lon"],
    );

    $form["meetup_events"][$type]["body"] = array(
      '#title' => t("Body Contents"),
      '#description' => 'The body content which will be synced to meetup (can use tokens if enabled)',
      '#type' => 'textarea',
      '#default_value' => ($settings[$type]["body"]) ? $settings[$type]["body"] : t("Come to our awesome meetup!")
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

  $form['meetup_api'] = array(
    '#type' => 'fieldset',
    '#tree' => TRUE,
    '#title' => t('Meetup.com API information'),
  );

  $api = variable_get('meetup_api', array());

  $form['meetup_api']['key'] = array(
    '#type' => 'textfield',
    '#title' => t('Meetup API Key'),
    '#default_value' => $api["key"],
    '#description' => t('The <a href="http://www.meetup.com/meetup_api/key/">meetup API key</a> for your user.')
  );

  $form['meetup_api']['group'] = array(
    '#type' => 'textfield',
    '#title' => t('Meetup Group ID'),
    '#default_value' => $api["group"],
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