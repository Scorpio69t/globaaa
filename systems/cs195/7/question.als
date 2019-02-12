sig Course {
  instructor: Faculty,
  enrolled: set Student
}
sig AssignmentID {}
sig Faculty {}
abstract sig Student {}
sig Undergrad extends Student {}
sig Grad extends Student {}

sig Handin {
  forCourse: Course,
  forAssign: AssignmentID,
  by: some Student
} {
  by in forCourse.enrolled
}

-- check to see if all students in a course submitted homework
pred question {
	some c : Course |
		all h : Handin |
			all s : Student |
				let submissions = h.forCourse in c |
					#(h.c) < #Student
					s in h.by		
}

run question
