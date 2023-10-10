BEGIN TRANSACTION;
EXPLAIN (analyse, buffers, verbose)
    select count(*) from flight.flights where departure like '%DEN%';
EXPLAIN (analyse, buffers, verbose)
    DELETE FROM flight.flights WHERE departure like '%DEN%';
EXPLAIN (analyse, buffers, verbose)
    select count(*) from flight.flights where departure like '%DEN%';
ROLLBACK;