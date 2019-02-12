select
  name,
  count(name) as count
from
  people_main
join
  people_likes
on
  id2 == id
group by
  name
order by
  count desc,
  name
;

select distinct
  name,
  count(name) as count
from
  people_likes
left join
  people_main
on
  id2 == id
group by
  name
order by
  count desc,
  name
;
