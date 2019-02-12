select
  occupation,
  count(occupation) as count
from
  (select distinct
     id2
   from
     people_likes) as pl
join
  people_main
on
  pl.id2 = id
group by
  occupation
order by
  count
limit
  2
;
