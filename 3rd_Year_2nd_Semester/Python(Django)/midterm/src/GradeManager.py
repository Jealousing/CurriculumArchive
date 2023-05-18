import struct
import datetime
import os

class GradeManagerMode:

    # 생성자
    def __init__(self):
        # 학번 학생이름 이메일 과목1~5점수 학과명 학과코드
        self.frmt = 'i30s30siiiii30si'
        self.frmtlen = struct.calcsize(self.frmt)
        self.SubjectFrmt = '30s'
        self.SubjectFrmtlen = struct.calcsize(self.SubjectFrmt)
        # 학생숫자
        self.number = int(1)
        # 학생정보 리스트
        self.StudentInfoList=[]
        # 관리자 로그인용
        self.ManagerID = '관리자'
        self.ManagerPW = '0000'


    # 학생정보 로드
    def LoadInfoList(self):
        # 빈 리스트
        StudentInfoList = []
        try:
            with open('StudentInfo.txt', 'rb') as f:        # 파일 오픈
                while True:
                    buffer = f.read(self.frmtlen)           # 읽기
                    if not buffer: break
                    info = struct.unpack(self.frmt, buffer) # unpack
                    StudentInfoList.append(info)            # 리스트에 정보추가
        except FileNotFoundError:
            print('파일이 없습니다')
        return StudentInfoList                              # 정보가 추가된 학생정보 리스트 리턴

    # 학생정보 저장
    def SaveInfoList(self, p_StudentInfoList):
        try:
            with open('StudentInfo.txt', 'wb') as f:        # 파일 오픈
                for info in p_StudentInfoList:
                    buffer = struct.pack(self.frmt, *info)  # packing
                    f.write(buffer)                         # 쓰기
        except FileNotFoundError:
            print('파일이 없습니다')

    # 학생추가
    def AddStudent(self):
        while True:
            # 학과코드 생성 및 초기화
            code = '000'
            # 학과 선택
            print("1.프로그래밍, 2.기획, 3.그래픽")
            select = int(input('추가할 학생의 학과를 선택해주세요'))
            # 선택에 따른 학과 및 학과코드 설정
            if select == 1:
                Department = '프로그래밍'
                code = '011'
            elif select == 2:
                Department = '기획'
                code = '022'
            elif select == 3:
                Department = '그래픽'
                code = '033'
            else:
                continue;

            # 학과 코드가 설정 되었으면 진행
            if code != '000':
                # 학생수를 불러오기 위한 txt 오픈
                fr = open('Number.txt', 'r')
                text = fr.read()

                # 비어있는지 확인
                if os.stat('Number.txt').st_size == 0:
                    self.number = int(1)
                else:
                    self.number = int(text)
                fr.close()

                # 입력을 받아옴
                name = input('학생의 이름을 입력해주세요')
                eMail = input('학생의 이메일을 입력해주세요')
                # 학번 설정
                id = int((datetime.datetime.now().strftime("%y")) + code + str(self.number).zfill(3))
                # 학생수 증가
                self.number += 1

                # 학생정보 저장용 튜플
                tempInfo = ()
                tempInfo += (int(id),)
                tempInfo += (name.encode(),)
                tempInfo += (eMail.encode(),)
                tempInfo += (int(-1),)
                tempInfo += (int(-1),)
                tempInfo += (int(-1),)
                tempInfo += (int(-1),)
                tempInfo += (int(-1),)
                tempInfo += (Department.encode(),)
                tempInfo += (int(code),)

                # 로드 추가 저장
                self.StudentInfoList = self.LoadInfoList()
                self.StudentInfoList.append(tempInfo)
                self.SaveInfoList(self.StudentInfoList)

                # 학생수 저장
                fw = open('Number.txt', 'w')
                fw.write(str(self.number))
                fw.close()
                
                # 추가진행 여부
                addInput = input('추가입력(y/n):')
                if addInput == 'N' or addInput == 'n':
                    break;



    # 불러오기 과목
    def LoadSubject(self, p_FileName):
        # 빈 리스트
        SubjectList = []
        try:
            with open(p_FileName, 'rb') as f:  # 파일 오픈
                while True:
                    buffer = f.read(self.SubjectFrmtlen)  # 읽기
                    if not buffer: break
                    info = struct.unpack(self.SubjectFrmt, buffer)  # unpack
                    SubjectList.append(info)  # 리스트에 정보추가
        except FileNotFoundError:
            print('파일이 없습니다')
        return SubjectList

    # 저장하기 과목
    def SaveSubject(self, p_FileName, p_SubjectList):
        try:
            with open(p_FileName, 'wb') as f:  # 파일 오픈
                for info in p_SubjectList:
                    buffer = struct.pack(self.SubjectFrmt, *info)  # packing
                    f.write(buffer)  # 쓰기
        except FileNotFoundError:
            print('파일이 없습니다')

    # 과목 추가
    def AddSubject(self):
        while True:
            SubjectList =[]
            # 학과 선택
            print("1.프로그래밍, 2.기획, 3.그래픽 4.종료")
            try:
                select = int(input('추가할 학생의 학과를 선택해주세요'))
            except:
                print("입력이 정확하지 않습니다")
            else:
                # 선택에 따른 파일이름 설정
                if select == 1:
                    SubjectFileName = 'ProgrammingSubject.txt'
                elif select == 2:
                    SubjectFileName = 'PlanningSubject.txt'
                elif select == 3:
                    SubjectFileName = 'GraphicSubject.txt'
                elif select == 4:
                    break;
                else:
                    continue;

                # 학과에 따른 과목 불러오기
                SubjectList = self.LoadSubject(SubjectFileName)
                
                # 리스트 길이로 판단
                if len(SubjectList) >= 5:
                    print("한 학과당 5과목을 초과할 수 없습니다.")
                    continue;
                else:
                    tempSubject = ()
                    tempSubject += (input('과목이름을 입력해주세요:').encode(),)
                    SubjectList.append(tempSubject)
                    self.SaveSubject(SubjectFileName, SubjectList)

                # 추가진행 여부
                addInput = input('추가입력(y/n):')
                if addInput == 'N' or addInput == 'n':
                    break;

    # 과목 삭제
    def DeleteSubject(self):
        while True:
            # 빈리스트 생성
            SubjectList = []

            # 학과 선택
            print("1.프로그래밍, 2.기획, 3.그래픽 4.종료")
            select = int(input('추가할 학생의 학과를 선택해주세요'))
            # 선택에 따른 파일이름 설정
            if select == 1:
                SubjectFileName = 'ProgrammingSubject.txt'
            elif select == 2:
                SubjectFileName = 'PlanningSubject.txt'
            elif select == 3:
                SubjectFileName = 'GraphicSubject.txt'
            elif select == 4:
                break;
            else:
                continue;

            # 학과에 따른 과목 불러오기
            SubjectList = self.LoadSubject(SubjectFileName)

            # 설정된 과목이 있는지 확인
            if len(SubjectList) == 0:
                print("설정된 과목이 없습니다.")
                continue;

            # 삭제할 과목선택
            print("삭제할 과목을 선택해주세요:")
            
            # 과목 목록 출력
            for i, value in enumerate(SubjectList, start=1):
                print(i,  value[0].decode().replace('\x00',''))

            # 선택
            select = int(input())

            if 0 < select < len(SubjectList)+1:
                # 삭제 및 저장
                del SubjectList[select - 1]
                self.SaveSubject(SubjectFileName, SubjectList)
            else:
                print("범위 밖입니다. 다시 학과부터 설정해주세요.")
                continue;

    # 관리자모드 메인
    def ManagerMode(self):
        # 로그인
        while True:
            # id pw 입력
            ManagerID = input('관리자의 아이디를 입력하세요: (종료: 0)')
            ManagerPW = input('관리자의 비밀번호를 입력하세요: ')

            # 일치하는지 확인
            if self.ManagerID == ManagerID:
                if self.ManagerPW == ManagerPW:
                    break
                else:
                    print("페스워드 오류입니다.")
                    continue;
            elif ManagerID == '0':
                break;
            else:
                print("없는 아이디입니다.")
                continue;



        while True:
            # 메뉴 출력
            print("\n\t< 관리자 모드 메뉴>")
            print("\t 1.학생추가")
            print("\t 2.과목추가")
            print("\t 3.과목삭제")
            print("\t 4.관리자 모드 종료")
            select = (int(input()))

            # 선택에 따른 함수 실행
            if select == 1:
                self.AddStudent()
            elif select == 2:
                self.AddSubject()
            elif select == 3:
                self.DeleteSubject()
            elif select == 4:
                break
            else:
                print("잘못된 입력을 하셨습니다.")
