select distinct
  name
from
  people_main
inner join
  people_likes
on
  people_likes.id2 = people_main.id
order by
  name; 
