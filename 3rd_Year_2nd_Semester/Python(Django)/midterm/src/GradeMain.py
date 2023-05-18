import GradeManager


def num_decorator(function):
    def wrapper(*arg, **kwargs):
        data=function(*arg,**kwargs)
        data=data+'33333'
        return data
    return wrapper()


class GradeProgram:
    
    # 생성자
    def __init__(self):
        self.ManagerMode = GradeManager
        self.StudentInfo_List = []

    # 점수에 따른 학점 계산 함수
    def ResultGrade(self, p_Grade):
        if int(p_Grade) >= 95:
            return 'A+'
        elif int(p_Grade) >= 90:
            return 'A'
        elif int(p_Grade) >= 85:
            return 'B+'
        elif int(p_Grade) >= 80:
            return 'B'
        elif int(p_Grade) >= 70:
            return 'C+'
        elif int(p_Grade) >= 60:
            return 'C'
        elif int(p_Grade) >= 50:
            return 'D+'
        elif int(p_Grade) >= 40:
            return 'D'
        else:
            return 'F'
        
    # 평점 계산 함수
    def AverageGrade(self,p_GradeList):
        sum = float(0)
        for i in p_GradeList:
            if int(i) >= 95:
                sum += 4.5
            elif int(i) >= 90:
                sum += 4.0
            elif int(i) >= 85:
                sum += 3.5
            elif int(i) >= 80:
                sum += 3.0
            elif int(i) >= 70:
                sum += 2.5
            elif int(i) >= 60:
                sum += 2.0
            elif int(i) >= 50:
                sum += 1.5
            elif int(i) >= 40:
                sum += 1.0
            else:
                sum += 0

        return sum / (len(p_GradeList))

    # 성적 출력 함수
    def ViewGrade(self):
        # 학생정보를 불러옴
        self.StudentInfo_List = self.ManagerMode.GradeManagerMode().LoadInfoList()
        
        while True:
            print("출력할 학과를 선택하세요")
            print("1.프로그래밍")
            print("2.기획")
            print("3.그래픽")
            print("4.종료")

            select = int(input())
            
            # 선택에 따른 학과 및 불러올 txt 파일 이름 설정
            if select == 1:
                selectDepartment = '프로그래밍'
                SubjectFileName = 'ProgrammingSubject.txt'
            elif select == 2:
                selectDepartment = '기획'
                SubjectFileName = 'PlanningSubject.txt'
            elif select == 3:
                selectDepartment = '그래픽'
                SubjectFileName = 'GraphicSubject.txt'
            elif select== 4:
                break;
            else:
                print("잘못입력 하셨습니다")
                continue;

            #성적출력
            #학과를 선택하게 하고 같은 학과의 모든 학생의 성적정보(학번, 이름, 각과목의 학점.., 평점)를 출력한다.
            for i in  self.StudentInfo_List:
                if i[8].decode().replace('\x00','') == selectDepartment:
                    print('학번: {0}'.format(i[0]), end=' ')
                    print('이름: {0}'.format(i[1].decode().replace('\x00', '')), end=' ')
                    print('이메일: {0}'.format(i[2].decode().replace('\x00', '')), end=' ')
                    # 과목 목록에 따른 성적 출력
                    gradeList = []
                    for index, value in enumerate(self.ManagerMode.GradeManagerMode().LoadSubject(SubjectFileName), start=1):
                        print(value[0].decode().replace('\x00', '')+"의 점수: "+self.ResultGrade(i[2+index]), end=' ')
                        gradeList.append(i[2+index])
                    # 과목들 점수로 평점 계산
                    print('평점: '+str(self.AverageGrade(gradeList)))

    # 성적 검색
    def SearchGrade(self):
        # 학생정보 불러옴
        self.StudentInfo_List = self.ManagerMode.GradeManagerMode().LoadInfoList()
        select = int(input('성적을 검색할 학번을 입력하세요'))
        
        # 입력한 학번과 같은 학번인 학생 찾기
        for i in self.StudentInfo_List:
            if int(i[0]) == select:
                # 기본정보 출력
                print('학번: {0}'.format(i[0]), end=' ')
                print('이름: {0}'.format(i[1].decode().replace('\x00', '')), end=' ')
                print('이메일: {0}'.format(i[2].decode().replace('\x00', '')), end=' ')

                # 과목 목록에 따른 성적 출력
                if str(i[8].decode().replace('\x00', '')) == '프로그래밍':
                    SubjectFileName = 'ProgrammingSubject.txt'
                elif str(i[8].decode().replace('\x00', '')) == '기획':
                    SubjectFileName = 'PlanningSubject.txt'
                elif str(i[8].decode().replace('\x00', '')) == '그래픽':
                    SubjectFileName = 'GraphicSubject.txt'

                gradeList = []
                # 과목 : 점수 출력
                for index, value in enumerate(self.ManagerMode.GradeManagerMode().LoadSubject(SubjectFileName), start=1):
                    print(value[0].decode().replace('\x00', '') + "의 점수: " + self.ResultGrade(i[2 + index]), end=' ')
                    gradeList.append(i[2 + index])

                # 과목들 점수로 평점 계산
                print('평점: ' + str(self.AverageGrade(gradeList)))

    # 성적입력
    def InputGrade(self):
        while True:
            # 학생 성적 불러옴
            self.StudentInfo_List = self.ManagerMode.GradeManagerMode().LoadInfoList()

            check = False
            print("성적입력입니다.")
            try:
                # 학번 입력 및 찾았는지 check
                id = int(input('성적 입력할 학생의 학번을 입력해주세요 (종료:0)'))
                for i in self.StudentInfo_List:
                    if int(i[0]) == id:
                        check = True
            except:
                # 예외
                print("입력이 정확하지 않습니다")
            else:
                if id == 0:
                    break;
                # 못찾았을 경우 다시 돌아감
                if check == False:
                    print("없는 학번을 입력하셨습니다.")
                    continue;

            # list에서 몇번째에 있는지 확인용
            findNum = int(0)
            
            for i in self.StudentInfo_List:
                findNum += 1
                if int(i[0]) == id:
                    # 튜플은 수정 불가능 하기때문에 list로 변경해서 수정 작업 예정
                    change = list(i) 
                    
                    # 기본정보 출력
                    print('이름: {0}'.format(i[1].decode().replace('\x00', '')))
                    print('학과: {0}'.format(i[8].decode().replace('\x00', '')))
                    
                    # 학과에 따른 과목 파일 이름 설정
                    if str(i[8].decode().replace('\x00', '')) == '프로그래밍':
                        SubjectFileName = 'ProgrammingSubject.txt'
                    elif str(i[8].decode().replace('\x00', '')) == '기획':
                        SubjectFileName = 'PlanningSubject.txt'
                    elif str(i[8].decode().replace('\x00', '')) == '그래픽':
                        SubjectFileName = 'GraphicSubject.txt'
                    
                    # 성적 입력 부분
                    for index, value in enumerate(self.ManagerMode.GradeManagerMode().LoadSubject(SubjectFileName), start = 1):
                        change[2+index] = int(input(value[0].decode().replace('\x00', '') + ": "))

                # 학번은 안겹치기 때문에 한번 진행후 종료
                break

            # 데이터 변경(입력)
            self.StudentInfo_List[findNum-1] = tuple(change)
            self.ManagerMode.GradeManagerMode().SaveInfoList(self.StudentInfo_List)

            # 추가진행 여부
            addInput = input('추가입력(y/n):')
            if addInput == 'N' or addInput == 'n':
                break;

    # 성적수정
    def ChangeGrade(self):
        while True:
            # 학생 성적 불러옴
            self.StudentInfo_List = self.ManagerMode.GradeManagerMode().LoadInfoList()
            check = False
            print("성적 수정입니다.")
            try:
                # 학번 입력 및 찾았는지 check
                id = int(input('성적 수정할 학생의 학번을 입력해주세요 (종료:0)'))
                for i in self.StudentInfo_List:
                    if int(i[0]) == id:
                        check = True
            except:
                # 예외
                print("입력이 정확하지 않습니다")
            else:
                if id == 0:
                    break;
                # 못찾았을 경우 다시 돌아감
                if check == False:
                    print("없는 학번을 입력하셨습니다.")
                    continue;

            # list에서 몇번째에 있는지 확인용
            findNum = int(0)

            for i in self.StudentInfo_List:
                findNum += 1
                if int(i[0]) == id:
                    # 튜플은 수정 불가능 하기때문에 list로 변경해서 수정 작업 예정
                    change = list(i)

                    # 기본정보 출력
                    print('성적을 수정할 학생의 이름: {0}'.format(i[1].decode().replace('\x00', '')))

                    # 학과에 따른 과목 파일 이름 설정
                    if str(i[8].decode().replace('\x00', '')) == '프로그래밍':
                        SubjectFileName = 'ProgrammingSubject.txt'
                    elif str(i[8].decode().replace('\x00', '')) == '기획':
                        SubjectFileName = 'PlanningSubject.txt'
                    elif str(i[8].decode().replace('\x00', '')) == '그래픽':
                        SubjectFileName = 'GraphicSubject.txt'

                    # 성적 입력 부분
                    for index, value in enumerate(self.ManagerMode.GradeManagerMode().LoadSubject(SubjectFileName),start=1):
                        change[2 + index] = int(input(value[0].decode().replace('\x00', '') + ": "))

                # 학번은 안겹치기 때문에 한번 진행후 종료
                break

            # 데이터 변경(입력)
            self.StudentInfo_List[findNum - 1] = tuple(change)
            self.ManagerMode.GradeManagerMode().SaveInfoList(self.StudentInfo_List)

            # 추가진행 여부
            addInput = input('추가입력(y/n):')
            if addInput == 'N' or addInput == 'n':
                break;


    # 메인 실행 부분
    def MainMenu(self):
        while True:
            # < 메인메뉴 >
            # 1.성적입력
            # 2.성적출력
            # 3.성적검색
            # 4.성적수정
            # 5.관리자모드
            # 6.종료

            print("\n학생 성적 관리 프로그램입니다.\n")
            print("\t< 메인메뉴>")
            print("\t 1.성적입력")
            print("\t 2.성적출력")
            print("\t 3.성적검색")
            print("\t 4.성적수정")
            print("\t 5.관리자모드")
            print("\t 6.종료")

            select = int(input())

            if select == 1:
                self.InputGrade()
            elif select == 2:
                self.ViewGrade()
            elif select == 3:
                self.SearchGrade()
            elif select == 4:
                self.ChangeGrade()
            elif select == 5:
                self.ManagerMode.GradeManagerMode().ManagerMode()
            elif select == 6:
                break;
            else:
                print("범위 밖입니다 다시 선택해주세요")
                continue;



