select distinct
    p1.name,
    p1.age
from
    people_main as p1
inner join
    people_main as p2
on
    p1.age > p2.age
inner join
    people_likes
on
    people_likes.id1 = p2.id
    and people_likes.id2 = p1.id
order by
    p1.name
;
