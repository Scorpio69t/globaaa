with
  subquery
as
  (select
    original_title,
    budget,
    release_date
  from
    movies
  where
    title == 'John Carter')
select
  movies.original_title,
  movies.budget,
  movies.release_date
from
  movies,
  subquery
where
  movies.release_date == date(subquery.release_date, '+9 days')
union
select
  *
from
  subquery;
