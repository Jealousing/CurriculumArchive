-- 현재 Development 부서에서 근무하는 직원의 이름과 생년월일을 조회
select d.dept_name, e.first_name, e.last_name, e.birth_date from employees e 
join dept_emp de on e.emp_no=de.emp_no
join departments d on de.dept_no=d.dept_no 
where d.dept_name = 'Development' and de.to_date='9999-01-01';

-- name이 Christ Muchinsky인 직원의 현재 소속 부서명과 현재 연봉( 이름, 소속 부서명, 연봉 )
select e.first_name, e.last_name,d.dept_name, s.salary from employees e 
join dept_emp de on e.emp_no=de.emp_no
join departments d on de.dept_no=d.dept_no 
join salaries s on s.emp_no = e.emp_no
where e.first_name='Christ' and e.last_name='Muchinsky' and s.to_date='9999-01-01';

-- 현재 title이 Senior Engineer로 일하고 있는 직원의 이름과 연봉
select e.first_name, e.last_name,t.title, s.salary from employees e
join titles t on t.emp_no=e.emp_no 
join salaries s on s.emp_no = e.emp_no
where t.title='Senior Engineer' and s.to_date='9999-01-01';

-- 부서별로 역대 매니저들의 인원수를 조회(부서명, 역대매니저 수)
select d.dept_name, count(dm.emp_no) as 매니저수 from departments d
join dept_manager dm on dm.dept_no = d.dept_no group by dm.dept_no;

select d.dept_name, count(*) from departments d
join dept_manager dm on d.dept_no=dm.dept_no group by d.dept_no;

select d.dept_name, cntmng.cnt from departments d
join (select dept_no, count(*) as cnt from dept_manager group by dept_no) as cntmng
on d.dept_no=cntmng.dept_no;

create view cntmng as (select d.dept_name, cntmng.cnt from departments d
join (select dept_no, count(*) as cnt from dept_manager group by dept_no) as cntmng
on d.dept_no=cntmng.dept_no);
-- 가장 자주 매니저가 바뀐 부서의 이름과 역대 매니저 수
select * from cntmng where cnt = (select max(cnt) from cntmng);

-- 각 부서별 매니저들의 현재 연봉
select d.dept_name,e.first_name,e.last_name ,s.salary from departments d
join dept_manager dm on d.dept_no = dm.dept_no
join employees e on e.emp_no=dm.emp_no
join salaries s on s.emp_no = e.emp_no
where dm.to_date = '9999-01-01' and s.to_date = '9999-01-01';


-- 각 부서별 최고 연봉을 받는 직원들의 이름과 연봉
create table emp_salary
(select de.dept_no, de.emp_no, s.salary from dept_emp de
join salaries s on de.emp_no=s.emp_no
where de.to_date = '9999-01-01' and s.to_date = '9999-01-01');

create view max_dept_salary as
select * from emp_salary where (dept_no,salary) in
(select dept_no,max(salary) from emp_salary group by dept_no);

select d.dept_name, e.first_name, e.last_name, ms.salary from departments d
join max_dept_salary ms on d.dept_no=ms.dept_no
join employees e on e.emp_no=ms.emp_no;

use cookdb;
create table buytbl2
(select * from buytbl);
alter table buytbl2 auto_increment = 100; -- 100 부터 시작