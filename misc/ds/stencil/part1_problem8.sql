select
  name,
  occupation
from
  people_friends
join
  people_main
on
  id2 = id
group by
  id
having
  count(id) >= 3
order by
  name
;
