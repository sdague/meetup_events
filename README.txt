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
doesn't provide that. There is a nodereference_tokens module provided
to use the body or teaser of referenced nodes as well.

You must also specify your API key from meetup.com, as well as the
numeric group id (which you can discover via the API console).

VENUES

As of 7.x-2.x meetup_events heuristically maps venues. There is a call
in the meetup API that returns the most recent venues that you have
used.

You can specify a venue_name_field (which is current only allowed to
be a node_reference). If the title of the location node matches one of
the recently used Venues, the meetup venue will be set to it. If not,
it will not be set (or changed).
