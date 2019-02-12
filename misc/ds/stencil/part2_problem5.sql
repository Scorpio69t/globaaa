select m1.original_title, m1.release_date, m1.revenue
from movies as m1
join
  (select m2.release_date, avg(m2.revenue) as average_revenue
   from movies as m2
   group by m2.release_date) as avg_table
on avg_table.release_date == m1.release_date
where m1.revenue > avg_table.average_revenue
order by m1.release_date asc, m1.revenue desc
;
