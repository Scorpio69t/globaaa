select
  occupation,
  count(occupation) as count
from
  people_main
group by
  occupation
order by
  count desc,
  occupation
;
