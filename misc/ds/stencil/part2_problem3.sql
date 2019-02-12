select distinct
  m1.original_title,
  case
    when exists
      /* I think this'll be slow since you'll have to execute this query
         for every row -- it would be better to join once with a subquery,
         but I'm having trouble doing that */
      (select m2.original_title
      from movies m2
      where m1.original_title != m2.original_title
            and m1.vote_average == m2.vote_average
	    and m1.runtime == m2.runtime)
    then 1
    else 0
  end as existent
from movies m1
order by
  m1.original_title
;

