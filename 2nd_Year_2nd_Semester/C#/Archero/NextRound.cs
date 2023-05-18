using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class NextRound : MonoBehaviour
{
    //다음라운드

    public static int Round; //라운드정보


    void Start()
    {
        Round = 0;
    }

    //충돌
    void OnCollisionEnter(Collision collision)
    {
        if (collision.transform.CompareTag("Player"))//플레이어
        {
            //0명일경우만
            if (PlayerTargeting.Player_Monster_List.Count == 0)
            {
                //이동
                Vector3 move = new Vector3(0, 0, this.transform.position.z + 30.0f);
                collision.transform.position = move;
            }

        }
    }


    // Update is called once per frame
    void Update()
    {
        Debug.Log("Round : "+Round);
    }
}
