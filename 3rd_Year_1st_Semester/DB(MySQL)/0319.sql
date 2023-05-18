-- 부서별로 역대 매니저가 몇명이였는지 조회 
select dept_no, count(*) as 매니저수 from dept_manager group by dept_no; -- dept_no에 따른 매니저수

-- 부서별로 역대 매니저가 두번이상 바뀐 부서 조회 
select dept_no, count(*) as 매니저수 from dept_manager group by dept_no having 매니저수>=3; -- 3명이상 조회
select dept_no, count(*) as 매니저수 from dept_manager group by dept_no having dept_no='d004';-- d004만조회

-- 부서별로 입사일이 1995년 이후인 사람들의 수를 조회
select dept_no, count(*) as 사람수 from dept_emp where from_date>='1995-01-01' group by dept_no;

-- 1985년 이후로 매니저가 두번이상 바뀐 부서를 조회
select dept_no, count(*) from dept_manager where from_date>='1985-01-01' group by dept_no having count(*)>=3;

-- 각 직원 별 최고 연봉을 조회
select emp_no, max(salary) from salaries group by emp_no;
select emp_no, max(salary) from salaries group by emp_no having max(salary)>=80000;
select emp_no, max(salary) from salaries group by emp_no having avg(salary)>=70000; -- 평균 연봉이 70000이상인사람의 최고연봉

-- 재직 중인 직원의 가장 최근 연봉 조회 
select emp_no, salary from salaries where to_date='9999-01-01';