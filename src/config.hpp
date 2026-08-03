#pragma once
#define MAX_RECONNECT_ATTEMPTS 20
#define AMQ_HOST "gdutils.clarifygdps.com"
#define AMQ_PORT 5672
#define AMQ_USER "gd"
#define AMQ_PASS "GeometryDashisahorizontalrunnerstylegamedevelopedandpublishedbyRobTopGames"

// Shared stream every client reads from (instead of one classic queue per client).
// Consumed over plain AMQP 0-9-1 (port 5672) via x-stream-offset, not the native
// Stream binary protocol (port 5552)
// on Android/iOS, so we stick with rabbitmq-c.
// Owned and declared server-side (x-max-age 60s), not by the client: the gd user
// only has read/write permissions on it, not configure.
#define AMQ_STREAM_NAME "rate-events-stream"
