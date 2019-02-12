select
  name
from
  people_main
where
  people_main.id not in (select distinct id2 from people_likes)
;
