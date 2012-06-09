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
  $query = db_select('node_type', 'n');
  $query->join('field_config_instance', 'fci', 'fci.bundle=n.type');
  $query->join('field_config', 'fc', 'fci.field_id=fc.id');
  $results = $query->fields('fc', array('field_name'))
    ->condition('fc.type', 'number_integer')
    ->execute();

  #$results = db_query("SELECT c.field_name, c.label from {content_node_field_instance} c INNER JOIN {node_type} n ON c.type_name=n.type where c.widget_module='number' and c.type_name='%s' ORDER BY n.name, c.label", $type);
  foreach ($results as $obj) {
    $fields[$obj->field_name] = $obj->field_name;
  }

  # then get all number fields that are on the nodereferences
  /* $results = db_query("SELECT c.field_name, c.label, cnf.global_settings from {content_node_field_instance} c INNER JOIN {node_type} n ON c.type_name=n.type INNER JOIN {content_node_field} cnf ON c.field_name=cnf.field_name where c.widget_module='nodereference' and c.type_name='%s' ORDER BY n.name, c.label", $type); */
  /* while($obj = db_fetch_object($results)) { */
  /*   $field = $obj->field_name; */
  /*   $label = $obj->label; */
  /*   $ref_types = unserialize($obj->global_settings); */

  /*   foreach ($ref_types["referenceable_types"] as $key => $value) { */
  /*     if ($value) { */
  /*       # and then see what numbers they have */
  /*       $ref_results = db_query("SELECT c.field_name, c.label from {content_node_field_instance} c INNER JOIN {node_type} n ON c.type_name=n.type where c.widget_module='number' and c.type_name='%s' ORDER BY n.name, c.label", $key); */
  /*       while($ref = db_fetch_object($ref_results)) { */
  /*         $fields[$field . ":" . $ref->field_name] = $label . " (node ref) => " . $ref->label; */
  /*       } */
  /*     } */
  /*   } */
  /* } */
  return $fields;
}

function meetup_events_get_date_fields_for_type($type) {
  $fields = array();
  $query = db_select('node_type', 'n');
  $query->join('field_config_instance', 'fci', 'fci.bundle=n.type');
  $query->join('field_config', 'fc', 'fci.field_id=fc.id');

  $results = $query->fields('n', array('name', 'type'))
    ->fields('fc', array('field_name'))
    ->fields('fci', array('data'))
    ->condition('fc.module', 'date')
    ->condition('n.type', $type)
    ->execute();

  foreach ($results as $obj) {
    $fields[$obj->field_name] = $obj->field_name;
  }
  return $fields;
}

function meetup_events_get_allowed_types() {
  $options = array();
  $query = db_select('node_type', 'n');
  $query->join('field_config_instance', 'fci', 'fci.bundle=n.type');
  $query->join('field_config', 'fc', 'fci.field_id=fc.id');

  $results = $query->fields('n', array('name', 'type'))
    ->fields('n', array('name', 'type'))
    ->condition('fc.module', 'date')
    ->execute();

  foreach($results as $obj) {
    $options[$obj->type] = $obj->name;
  }
  return $options;
}

/***
 * Form for main settings page
 */


function meetup_events_get_settings() {
  $settings = variable_get("meetup_events", array());
  $options = meetup_events_get_allowed_types();

  foreach ($options as $type => $name) {
    $key = $type . "_enabled";
    if (!array_key_exists($key, $settings)) {
      $settings[$key] = FALSE;
    }

    if (!array_key_exists($type, $settings)) {
      $settings[$type] = array(
        'date_field' => NULL,
        'body' => t("Come to our awesome meetup!"),
        'venue_field' => NULL,
        'venue_lat' => NULL,
        'venue_lon' => NULL,
      );
    }

  }

  return $settings;
}

function meetup_events_admin_settings() {
  ctools_include('dependent');

  $options = meetup_events_get_allowed_types();

  $settings = meetup_events_get_settings();

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

    $dashtype = str_replace("_", "-", $type);
    $form["meetup_events"][$type] = array(
      '#type' => 'fieldset',
      '#input' => TRUE,
      '#id' => 'group-' . $dashtype,
      '#prefix' => '<div id="group-' . $dashtype. '-wrapper">', # <div id="group-' . $type . '">',
      '#suffix' => '</div>', # </div>',
      '#dependency' => array("edit-meetup-events-" . $dashtype . "-enabled-" . $dashtype => array(TRUE))
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

  $api = variable_get('meetup_api', array(
           'key' => "",
           'group_url' => "",
           'oauth_key' => "",
           'opts' => array(),
         ));

  $form['meetup_api']['key'] = array(
    '#type' => 'textfield',
    '#title' => t('Meetup API Key'),
    '#default_value' => $api["key"],
    '#description' => t('The <a href="http://www.meetup.com/meetup_api/key/">meetup API key</a> for your user.')
  );

  $form['meetup_api']['group_url'] = array(
    '#type' => 'textfield',
    '#title' => t('Meetup Group URL'),
    '#default_value' => $api["group_url"],
    '#description' => t('The meetup group url. This will be the name of your group after http://meetup.com/<u><b>Group-URL</b></u>'),
  );

  $form['meetup_api']['oauth_key'] = array(
    '#type' => 'textfield',
    '#title' => t('OAuth Key'),
    '#default_value' => $api["oauth_key"],
    '#description' => t('If you want to enable RSVP dynamic widgets, you need to register for an oauth id at meetup and add it here'),
  );

  $api["opts"] = ($api["opts"]) ? $api["opts"] : array();

  $form['meetup_api']['opts'] = array(
    '#title' => t('Advanced Options'),
    '#type' => 'checkboxes',
    '#options' => array(
      'force_to_meetup' => 'Force RSVPs to Meetup.com',
    ),
    '#default_value' => $api["opts"],
    '#description' => t('If your RSVPs require questions to be answered, you need to check this box to foce them to the meetup site to RSVP, otherwise you won\'t get that information.')
  );

  return system_settings_form($form);
}

function meetup_events_admin_settings_validate($form, &$form_state) {
  $values = $form_state["values"];

  $request_url = "https://api.meetup.com/2/groups?";
  $request_headers = array(
    'Content-Type' => 'application/x-www-form-urlencoded',
    'Accept-Charset' => 'utf-8');
  $request_method = 'GET';

  $rawdata = array(
    "key" => $values['meetup_api']['key'],
    "group_urlname" => $values['meetup_api']['group_url'],
    "page" => 20,
  );
  $request_url .= http_build_query($rawdata, '', '&');

  # TODO: validate the key seperately than group_url?

  $res = drupal_http_request($request_url, $request_headers, $request_method, '');
  $resp = json_decode($res->data);

  foreach ($resp->results as $result) {
    if ($result->urlname == $values['meetup_api']['group_url']) {
      $form_state["values"]["meetup_api"]["group"] = $result->id;
      break;
    }
  }

  # TODO: other validations we should make?
  # if we didn't find an id, assume things are wrong
  if (!$form_state["values"]["meetup_api"]["group"]) {
    form_set_error('meetup_api][group_url', t("Must specify valid group url"));
  }
}
