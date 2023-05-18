select emp_no, birth_date, first_name from employees;
select * from employees where gender = 'F'and birth_date>='1950-01-01'and birth_date<='1959-12-31';
select * from employees where birth_date>='1960-01-01'and birth_date<='1969-12-31' or hire_date >='1990-01-01';
select * from employees where hire_date in ('1991-08-30','1991-12-01');
select * from employees where first_name like 'A%';
select * from titles;
select distinct title from titles;
select * from titles where title in ('Senior Engineer','Senior Staff');