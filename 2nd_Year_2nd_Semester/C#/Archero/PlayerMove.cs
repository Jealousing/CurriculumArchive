using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class PlayerMove : MonoBehaviour
{
    //플레이어 이동설정

    Vector3 movement;                   //이동용
    public static Quaternion PlayerRot; //플레이어 rotation
    Rigidbody rigidbody;                     //rigidbody

    //플레이어 이동속도 회전속도
    public float PlayerMoveSpeed = 10f;
    public float RotateSpeed = 10f;

    //키보드 입력용
    float horizMove;
    float vertiMove;


    //save
    Vector3 savePos;

    //움직이는중
    public static bool Isruning = false;

    void Awake()
    {
        Screen.SetResolution(800, 1200, true);
        rigidbody = GetComponent<Rigidbody>();
    }

    void Update()
    {
        if (RoomSetting.IsRun)//게임진행중일때만 
        {
            //키보드입력
            horizMove = Input.GetAxisRaw("Horizontal");
            vertiMove = Input.GetAxisRaw("Vertical");
        }
    }

    private void FixedUpdate()
    {
        if(RoomSetting.IsRun)//게임진행중일때만 
        {
            Run();
            Turn();
        }
    }

    //충돌
    void OnCollisionEnter(Collision collision)
    {
        if (collision.transform.CompareTag("Monster"))
        {//몬스터에게 충돌했을경우 
            PlayerData.Player_Current_Hp -= 200;
        }
    }

    //움직이는 부분
    void Run()
    {
        movement.Set(horizMove, 0, vertiMove);
        movement = movement.normalized * PlayerMoveSpeed * Time.deltaTime;

        rigidbody.MovePosition(transform.position + movement);
        PlayerRot = this.transform.rotation;
        if (horizMove == 0 && vertiMove == 0)
        {
            this.transform.position = savePos;
            rigidbody.velocity = new Vector3(0f, 0f, 0f); ;
            movement = new Vector3(0f, 0f, 0f);
            Isruning = false; 
        }
        else
        {
            savePos = this.transform.position;
            Isruning = true;
        }
            
    }
    //회전부분
    void Turn()
    {
       
        if (horizMove == 0 && vertiMove == 0)
        {
            return;
        }
        if (Isruning)
        {
            Quaternion newRotation = Quaternion.LookRotation(movement);

            rigidbody.rotation = Quaternion.Slerp(rigidbody.rotation, newRotation, RotateSpeed * Time.deltaTime);
        }
       
        
    }
}
