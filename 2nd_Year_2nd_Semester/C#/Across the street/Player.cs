using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Player : MonoBehaviour
{
    public void _OnClick_Restart()  //재시작 버튼을 누른 경우 실행
    {
        transform.position = new Vector3(0f, 1f, 0f);   //처음 시작위치로 초기화
        Stamina = 100f;                                                 //스테미나를 다시 풀로 채워줌
        PlayerMoveSpeed = savemovespeed;                //초기 설정된 플레이어의 속도로 설정
    }

    Vector3 movement;   //좌표
    Rigidbody rigidbody;    //움직임을 위해서
    Animator animator;    //애니메이션 설정을 위해서
    float animator_timer = 0.0f; //Idle 모션을 추가하기위한 타이머

    public float PlayerMoveSpeed = 10f; //플레이어 이동속도
    public float JumpPower = 10f;   //점프력
    public float RotateSpeed = 10f; //화면 회전 속도

    float savemovespeed;    //초기속도 저장용 변수

    bool isJumping; //점프여부
    bool IsRunning; //달리기 여부

    float horizontalMove;   //가로 움직임 변수
    float verticalMove;       //세로 움직임 변수

    public float Stamina;   //스테미나를 저장할 변수
    public float StaminaTime = 1.0f;    //스테미나가 어느 시간마다 증감될지 시간 설정
    float timer = 0.0f;                         //타이머

    void Start()
    {
        Stamina = 100f; //스테미나 초기설정
        savemovespeed = PlayerMoveSpeed;    //초기 속도 저장
    }
    void Awake()
    {
        Screen.sleepTimeout = SleepTimeout.NeverSleep;//실행중 화면꺼짐x
        Screen.SetResolution(1280, 720, false);//화면크기설정
        rigidbody = GetComponent<Rigidbody>(); //참조Rigidbody
        animator = GetComponent<Animator>(); //참조Animator
    }
    void FixedUpdate() // 동일 시간반복 
    {
        if (GameManager_.PlayerDie|| !GameManager_.GameStart)   //게임시작전이거나, 플레이어가 죽었으면 실행취소
            return;

        Run();  //걷기
        Turn(); //회전
    }

    private void OnCollisionEnter(Collision collision)  //충돌
    {
        if (collision.transform.tag == "Ground"|| collision.transform.tag =="Ship") //땅이나 배위에 착지했을경우 다시 점프가능하게 설정
            isJumping = false;
    }

    void Update() //프레임기반 반복
    {
        if (GameManager_.PlayerDie || !GameManager_.GameStart)//게임시작전이거나, 플레이어가 죽었으면 실행취소
        {
            IsRunning = false;  //혹시라도 달리다 죽었을 경우를 위해 달리기 취소
            return;
        }
        //키입력
        horizontalMove = Input.GetAxisRaw("Horizontal"); 
        verticalMove = Input.GetAxisRaw("Vertical");

        if (Input.GetButtonDown("Jump"))//점프할경우
        {
            if(!isJumping)
            {
                isJumping = true;//점프여부 True
                Jump();                //점프함수 실행
            }
        }

        //LeftShitf를 누르면 달리기 실행
        if (Input.GetKeyDown(KeyCode.LeftShift) && Stamina >= 12)   
        {
            Stamina -= 12f; //스테미나 감소
            PlayerMoveSpeed = savemovespeed * 1.75f;    //초기속도의 1.75배만큼 빨라짐
            IsRunning = true;   //달리기 활성화상태
        }
        //때면 실행 취소
        if (Input.GetKeyUp(KeyCode.LeftShift))
        {
            if (IsRunning)
            {
                PlayerMoveSpeed = savemovespeed;    //원래의 속도로 되돌림
                IsRunning = false;                                  //달리기 취소
            }

        }

        timer += Time.deltaTime;    //일정 시간마다 증가
        if (timer > StaminaTime)    //설정된 시간을 넘어갔을 경우 실행
        {
            GameManager_.ScoreSet(transform.position.z);    //스코어를 플레이어 위치에따라 설정

            if (Stamina < 15)//스테미너가 15이하면 달리기 취소
            {
                PlayerMoveSpeed = savemovespeed;
                IsRunning = false;
            }

            if (IsRunning)//달리기 중일경우 일정 시간마다 12씩 감소
            {
                Stamina -= 12f;
            }
            if (!IsRunning && Stamina < 100)    //달리지 않을경우 스테미나 증가 최대치는 100
            {
                Stamina += 4f;
            }

            if (savemovespeed > PlayerMoveSpeed)    //설정된 속도보다 낮아질 경우 원래의 속도로 변환
                PlayerMoveSpeed = savemovespeed;

            GameManager_.StaminaSet(Stamina);   //출력용 스테미나 전달


            timer = 0;//타이머 초기화
        }

        AnimationUpdate();//애니메이션 업데이트
    }

    void Run()  //상하좌우 움직임 설정
    {
        movement.Set(horizontalMove, 0, verticalMove);//좌표 설정
        movement = movement.normalized * PlayerMoveSpeed * Time.deltaTime; //일정

        rigidbody.MovePosition(transform.position + movement);//자연스러운 이동을위한 MovePosition 사용
    }

    void Jump() //점프 설정
    {
        if (!isJumping) //점프
            return;
        //AddForce는 rigidbod를 이용해 이동 -> 부드럽게 가능
        rigidbody.AddForce(Vector3.up * JumpPower, ForceMode.Impulse); 
    }
    void Turn() //회전 설정
    {
        if (horizontalMove == 0 && verticalMove == 0)//방향키 제어가 없을 땐 실행 x
            return;
        Quaternion newRotation = Quaternion.LookRotation(movement);//백터가 가진 방향으로 앞을 보는 회전 값 생성
        rigidbody.rotation = Quaternion.Slerp(rigidbody.rotation, newRotation, RotateSpeed * Time.deltaTime); //회전 속도에따라 천천히 회전
    }
    void AnimationUpdate()
    {
        if (horizontalMove == 0 && verticalMove == 0) //방향키 제어 x Idle1
        {
            animator.SetBool("IsRunning", false);
            animator_timer += Time.deltaTime;
        }
        else//뛰는 모션
        {
            animator.SetBool("IsRunning", true);
            animator_timer = 0;
        }

        if(animator_timer>3.0f)//idle 상태로 오래있을경우 실행
        {
            animator.SetBool("Idle2", true);
        }
        else
        {
            animator.SetBool("Idle2", false);
        }
    }
}
