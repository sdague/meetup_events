<?php

/**
 * Implementation of hook_views_api().
 */
/* function meetup_events_views_api() { */
/*   return array( */
/*     'api' => 2, */
/*     'path' => drupal_get_path('module', 'meetup_events'), */
/*   ); */
/* } */
  /**
   * Implementation of hook_views_data().
   */
function meetup_events_views_data() {
    $data;
    $data['meetup_events']['table']['group'] = t('Meetup Events');
    $data['meetup_events']['table']['base'] = array(
      'field' => 'id',
      'title' => t("Meetup Events"),
      'help' => t("Info associated with meetup event")
    );

    $data['meetup_events']['table']['join']['node'] = array(
      'left_field' => 'nid',
      'field' => 'nid'
    );

    $data['meetup_events']['id'] = array(
      'title' => t("Meetup Event id"), // The item it appears as on the UI,
      'help' => t("No help"), // The help that appears on the UI,
      // Information for displaying a title as a field
      'field' => array(
        'field' => 'id', // the real field
        'handler' => 'views_handler_field_numeric',
        'click sortable' => TRUE,
      ),
      'filter' => array('handler' => 'views_handler_filter_numeric'),
      'sort' => array('handler' => 'views_handler_sort'),
    );

    # TODO: add a custom views_handler to generate the RSVP url

    _meetup_events_define_view_reference($data, 'meetup_events', 'nid', 'Meetup Id', 'Meetup event id for the node', 'node', 'nid');

    return $data;
}

function _meetup_events_define_view_reference(&$data, $table, $column, $title, $help, $base, $field) {
  $data[$table][$column] = array(
    'title' => $title, // The item it appears as on the UI,
    'help' => $help, // The help that appears on the UI,
    'field' => array(
      'field' => $column, // the real field
      'handler' => 'views_handler_field_numeric',
    ),
    // Information for displaying a title as a field

    'relationship' => array(
      'base' => $base,
      'field' => $field,
      'handler' => 'views_handler_relationship',
      'label' => $title,
    ),
  );
}

/**
 * Implementation of hook_views_post_render().
 *
 * The trick of this is that if the user manages to construct the right meetup
 * URL in a view, the javascript still isn't loaded. So we look for nodes in views
 * that have meetup ids, and in those cases load the javascript.
 *
 * TODO: this is a bit more agressive than it should be, might be able to
 * narrow down when the RSVP js should be shown.
 */
function meetup_events_views_post_render(&$view, &$output, &$cache) {
  $api = variable_get('meetup_api', array());

  # if we don't have an oauth key, none of this is going to work
  $oauth_id = $api["oauth_key"];
  if (!$oauth_id) {
    return;
  }

  foreach ($view->result as $result) {
    $meetup_id = meetup_events_get_meetupid($result->nid);
    if ($meetup_id) {
      // this is a bit tricky, because doing add_js to add a setting twice
      // does a merge, we use this relatively undocumented trick to fetch
      // the existing js, and make sure we haven't added a setting yet.

      // TODO: we only use meetup_events setting for oauth_key, but if that
      // changes, so would this.
      $currentjs = drupal_add_js(NULL, NULL, 'header');

      foreach ($currentjs['setting'] as $setting) {
        if ($setting['meetup_events']) {
          return;
        }
      }

      drupal_add_js(
        array('meetup_events' =>
          array('oauth_key' => $api["oauth_key"])),
        'setting'
      );

      drupal_add_js(drupal_get_path('module', 'meetup_events') . '/js/rsvp.js');
    }
  }
}