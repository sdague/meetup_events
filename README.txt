OVERVIEW

The point of this module is to synchronize events from a drupal
instance to a meetup.com group. This uses the meetup API.

There are lots of rough edges in this code still, so you might need to
dive into the code if you happen to hit one of those.

INSTALLATION

After installation, go to the administration panel
(/admin/settings/meetup_events) and select which types have date
fields that you want to sync. Only types with date fields are
syncable.

From the admin panel you can also specify which date field to use for
the type, as well as what the body text should be (you can use
tokens). If you have 'notification_content' installed you'll have a
[node-body] token available, which you'll really want, default Token
doesn't provide that.

You must also specify your API key from meetup.com, as well as the
numeric group id (which you can discover via the API console).

HOW TO SPECIFY VENUES

Specifying Venues is still a bit of black magic because there is no
really standard way to do this. In my sites I use a node_reference to
a Location type so that I don't need to repeat venues often (we have a
few dozen events a year, but only 3 venues that get used).

If you want the module to sync venues you need to build a theme
function that will handle it.

* theme_meetup_events_venueid($node)

Given a fully populated $node, return the venueid as an
integer. Optionally return venue_id:lat:lon as a string if you want to
force a specific lat lon.

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

