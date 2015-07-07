PRAGMA foreign_keys=OFF;
BEGIN TRANSACTION;
CREATE TABLE log ( id text, timestamp datetime, humidity real, temperature real, mq9val real, dn7val real);
CREATE TABLE ids (id text, latitude real, longitude real, first datetime);
CREATE INDEX log_id on log (id);
CREATE INDEX ids_id on ids (id);
COMMIT;
