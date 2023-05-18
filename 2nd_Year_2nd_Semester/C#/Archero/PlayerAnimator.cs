using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class PlayerAnimator : MonoBehaviour
{
    //에니메이션 설정

    private Animator anim;

    void Start()
    {
        anim = GetComponent<Animator>();
    }
    void FixedUpdate()
    {
        if (!RoomSetting.IsRun)//진행을 안하고잇으면 return 
        {
            return;
        }
            if (Input.GetKey(KeyCode.UpArrow)||Input.GetKey(KeyCode.W))
        {
            anim.SetBool("Run", true);
        }
        else
        {
            anim.SetBool("Run", false);
        }

        if (Input.GetKey(KeyCode.LeftArrow) || Input.GetKey(KeyCode.A))
        {
            anim.SetBool("RunLeft", true);
        }
        else
        {
            anim.SetBool("RunLeft", false);
        }

        if (Input.GetKey(KeyCode.RightArrow) || Input.GetKey(KeyCode.D))
        {
            anim.SetBool("RunRight", true);
        }
        else
        {
            anim.SetBool("RunRight", false);
        }

        if (Input.GetKey(KeyCode.DownArrow) || Input.GetKey(KeyCode.S))
        {
            anim.SetBool("RunBack", true);
        }
        else
        {
            anim.SetBool("RunBack", false);
        }

        if (PlayerTargeting.IsAttack)
        {
            anim.SetBool("Attack", true);
        }
        else
        {
            anim.SetBool("Attack", false);
        }
    }
}
