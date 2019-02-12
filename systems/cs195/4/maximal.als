abstract sig Need {}

one sig EnoughSleep extends Need {}
one sig GoodGrades extends Need {}
one sig SocialLife extends Need {}
sig Person { choice : set Need }

fact { 
	all p:Person | {
    	some p.choice
    		#(p.choice) < 3
	}
}

run {lone Person} for 3 Person

run {lone Person and some p: Person | #(p.choice) < 2 } for 3 Person

run {lone Person and some p: Person | #(p.choice) < 1 } for 3 Person
