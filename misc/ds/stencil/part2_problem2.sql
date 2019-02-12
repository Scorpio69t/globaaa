select
  count(title)
from
  movies
where
  /* these seem to be acting case-insensitve, even though they shouldn't 
   * could use ilike in postgres or convert all titles to lowercase */
  title like 'The%'
  or title like '%2'
  or title like '%shark%'
;
