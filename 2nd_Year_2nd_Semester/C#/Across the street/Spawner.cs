using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Spawner : MonoBehaviour
{
    public GameObject Target;           //스폰시킬 타겟불러오기
    public GameObject Player_data;  //플레이어의 위치데이터를 위해 불러오기

    public float SpawnTime = 5.0f;  //스폰반복 시간

    float timer = 0.0f; //타이머
    void Start()
    {
        timer = SpawnTime - 1f; //처음에 빠르게 생성하기위해 설정
    }

    void SpawnCar()
    {
        GameObject tempobj = Target;//복사할 정보

        GameObject copyobj = GameObject.Instantiate(tempobj);   //복사
        copyobj.SetActive(true);    //활성화
        copyobj.transform.position = Target.transform.position; //위치설정

    }

    // Update is called once per frame
    void Update()
    {
        if (GameManager_.PlayerDie || !GameManager_.GameStart) //죽거나 게임시작을 안했을경우 return
            return;

        if (Vector3.Distance(Player_data.transform.position,this.transform.position)<=40f)  //위치차이가 40보다 적을경우에만 실행
        {
            timer += Time.deltaTime;    //타이머
            if (timer > SpawnTime)  //설정된 시간을 넘겼을경우 스폰!
            {
                SpawnCar(); //스폰실행
                timer = 0;  //타이머 초기화
            }
        }
        
    }
}
