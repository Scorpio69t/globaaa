select distinct
  pls.id1,
  p1.name,
  pls.id2,
  p2.name
from
  people_likes as pls
left join
  /* where A likes and is friends with B */
  (select
    pl.id1,
    pl.id2
  from
    people_likes as pl
  join
    people_friends as pf
  on
    pl.id1 == pf.id1
    and pl.id2 == pf.id2) as likes_friends
on
  /* exclude lines not in likes_friends */
  pls.id1 == likes_friends.id1
  and pls.id2 == likes_friends.id2
join
  people_main as p1
on
  /* get name info */
  p1.id == pls.id1
join
  people_main as p2
on
  p2.id == pls.id2
where
  likes_friends.id1 is null
order by
  pls.id1 asc,
  pls.id2 asc
;

/*
select distinct
  p1.id,
  p1.name,
  pl.id2,
  p2.name
from
  people_main as p1
join
  people_likes as pl
on
  p1.id = pl.id1
join
  people_friends as pf
on
   * I don't think this is enough: consider the case where likes (A, B)
   * and (A, C) and then friends (A, B), (A, D). this should only return (A, C)
   * however I think it'd also return (A, B) [since it could join with (A, D)] 
  p1.id = pf.id1
  and pf.id2 != pl.id2
join
  people_main as p2
on
  p2.id = pl.id2
order by
  p1.name,
  p2.name
;
*/
