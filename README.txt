OVERVIEW

The point of this module is to synchronize events from a drupal
instance to a meetup.com group. This uses the meetup API.

There are lots of rough edges in this code still, so you might need to
dive into the code if you happen to hit one of those.

INSTALLATION

After installation, go to the administration panel
(/admin/settings/meetup_events) and select which types have date
fields that you want to sync.

You must also specify your API key from meetup.com, as well as the
numeric group id (which you can discover via the API console).

HOW TO SPECIFY RELATIONSHIPS

Everyone builds events differently, so building a complex
administration panel for this didn't make a lot of sense. Instead I'm
cheating and using the theme infrastructure.

You must implement 3 theme functions for your site:

* theme_meetup_events_body($node)

Given a fully populated $node, return the text (basic html markup
allowed) of the body. This could be as simple as $node->body, or it
could include other fields.

* theme_meetup_events_date($node)

Given a fully populated $node, return the start date/time.

* theme_meetup_events_venueid($node)

Given a fully populated $node, return the venueid as an
integer.

VENUES

Meetup has no way of creating the venues programatically, which is
probably good.

So the basic flow is as follows:
* create the event on drupal
* see that the event was synced to meetup.com
* set a venue on meetup.com for the event
* copy that venue id back to a cck field somewhere in drupal

If you're using a location type which is a node reference from your
events, this works out very well, as you need to build this location
-> venue link only once, future events at that venue will
automatically get synced.

