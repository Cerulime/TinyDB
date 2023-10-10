EXPLAIN (analyse, buffers, verbose)
    update flight.flights set departure = 'BBC' where departure like '%DEN%';
EXPLAIN (analyse, buffers, verbose)
    update flight.flights set departure = 'DEN' where departure like '%BBC%';