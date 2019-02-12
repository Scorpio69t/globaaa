select 
  case
    when review == 'awful' or review == 'poor' then 'do not watch'
    else original_title
  end as original_title,
 vote_average, review 
from movies
join scores
on vote_average between min_score and max_score
order by id asc
;
