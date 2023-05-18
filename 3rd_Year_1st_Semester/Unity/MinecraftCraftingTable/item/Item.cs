using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Item : MonoBehaviour
{
    /*
     플레이도중 아이템이 필드에 드랍이 발생하면 아이템을 오브젝트형식으로 만들어주는 스크립트이다.
     */

    //오브젝트에 저장되어잇을 정보들
    public ItemData m_itemData; //아이템의 데이터정보
    public int m_ItemNum;           //갯수정보

    //드랍이 완료될때까지 대기용
    bool IsSetting = false;

    //드랍이 발생하면
    private void Start()
    {
        //드랍의 시작은 캐릭터 머리위가 되며
        this.transform.position = new Vector3(this.transform.position.x, this.transform.position.y + 1f, this.transform.position.z);
        //드랍아이템의 이미지는 아이템 데이터의 아이콘을 받아와서 사용하고
        this.transform.GetChild(0).GetComponent<SpriteRenderer>().sprite = m_itemData.itemIcon;
        //아이템이 떨어질 위치를 정해줄 랜덤값을 받아옴
        var RandForce = RandomForce();
        //받아온 랜덤값을 토대로 그쪽방향으로 버리는것처럼 발생
        this.transform.GetComponent<Rigidbody>().AddForce(new Vector3(RandForce.RandX, 1, RandForce.RandZ) *20);
        //버려지고나서 아이템이 회전하도록 설정
        InvokeRepeating("ItemTurn", 1, 0.025f);
    }

    //아이템 회전
    void ItemTurn()
    {
        //세팅완료
        IsSetting = true;

        //회전시작
        this.transform.Rotate(new Vector3(0, 2, 0));
    }

    //랜덤값 생성 
    private (int RandX,int RandZ) RandomForce()
    {
        //-1~1 사이의 값 랜덤생성
        int randomX=Random.Range(0,3)-1;
        int randomZ;

        //x의 랜덤값이 0일경우
        if(randomX==0)
        {
            //-1, 1둘중 하나만 나오도록 설정
            randomZ = Random.Range(0, 2);
            if (randomZ == 0)
                randomZ = -1;
        }
        else
        {
            //-1~1사이랜덤값 생성
            randomZ = Random.Range(0, 3) - 1;
        }

        //랜덤값 리턴
        return (randomX, randomZ);

    }
    
    //충돌시 인벤토리에 아이템 등록처리
    private void OnCollisionEnter(Collision collision)
    {
        if(collision.transform.tag=="Player" && IsSetting)
        {
            PlayerControl.GetInstance.GetComponent<InventorySystem>().SetItem(m_itemData,m_ItemNum);
            Destroy(this.gameObject);
        }
    }
    private void OnCollisionStay(Collision collision)
    {
        if (collision.transform.tag == "Player" && IsSetting)
        {
            PlayerControl.GetInstance.GetComponent<InventorySystem>().SetItem(m_itemData, m_ItemNum);
            Destroy(this.gameObject);
        }
    }


}
