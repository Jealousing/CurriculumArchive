using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.EventSystems;
using UnityEngine.UI;

//아이템 슬롯관리 스크립트
public class ItemSlotControl : MonoBehaviour, IPointerDownHandler, IPointerUpHandler, IDragHandler, IPointerEnterHandler, IPointerExitHandler
{
    
    public ItemData m_itemData;                                 //아이템 데이터
    public Image m_itemImage;                                   //아이템 이미지
    public E_ItemType m_SlotType;                             //아이템 
    public RectTransform m_itemTransform;            //아이템 위치정보
    public Text m_text;                                                //아이템 갯수 텍스트
    private CanvasGroup m_canvasGroup;                  //마우스 클릭 막기용
    public Canvas m_canvas;                                       //켄버스 정보
    public int m_itemNum;                                           //아이템 갯수


    RecipeManager m_recipeManager;                        //레시피 참조
    GameObject m_setDownPoint;                               //클릭 지점

    public GameObject m_DropObject;                        //아이템을 드랍할때 복사할 오브젝트
    public GameObject m_itemToolTip;                       //아이템설명을 띄울때 가져올 오브젝트


    void Start()
    {
        //컴퍼넌트 가져오기
        m_canvasGroup = GetComponent<CanvasGroup>();
        //슬롯 업데이트 (데이터에 맞게 슬롯 갱신)
        UpdateSlotData();

        //데이터가 들어있으면
        if (m_itemData != null)
        {
            //아이템 코드에 따라
            if (m_itemData.itemType==E_ItemType.Normal)
            {
                //갯수 출력
                m_text.text = m_itemNum.ToString();
            }
            else
            {
                //미출력 (낚싯대, 도끼 , 당근낚싯대)
                m_text.text = "";
            }
        }
        else
        {//데이터 없으면 출력x
            m_text.text = "";
        }
    }

    public void UpdateSlotData() //슬롯 정보 업데이트
    {
        //자기 상위 오브젝트가 레시피매니저를 가지고있을경우
        if (this.gameObject.transform.parent.GetComponent<RecipeManager>())
        {
            //추가 제작 멈추도록 설정
            m_recipeManager = this.gameObject.transform.parent.GetComponent<RecipeManager>();
            m_recipeManager.SetRecipeBool();
        }
        //데이터가 있으면
        if (m_itemData != null)
        {
            //슬롯 갱신
            m_itemImage.gameObject.SetActive(true);
            m_itemImage.sprite = m_itemData.itemIcon;
            if (m_itemData.itemcode < E_ItemCode.FishingRod) 
            {
                //아이탬 갯수
                m_text.text = m_itemNum.ToString();
            }
            else
            {
                //미출력
                m_text.text = "";
            }
        }
        else//데이터가 없을경우
        {
            m_itemImage.gameObject.SetActive(false);
            m_itemImage.sprite = null;
            m_text.text = "";
        }

        //제자리?
        m_itemTransform.anchoredPosition = Vector3.zero;
    }

    //클릭하는 순간
    public void OnPointerDown(PointerEventData p_eventData)
    {
        //눌린버튼이 좌클릭일경우
        if (p_eventData.button == PointerEventData.InputButton.Left)
        {
            //레이케스트 차단 
            //false = 레이케스트 충돌 x 클릭을 못함
            //true = 클릭 가능
            m_canvasGroup.blocksRaycasts = false;
            
            //눌린 곳에 게임오브젝트 정보 받아옴
            foreach (GameObject selectobj in p_eventData.hovered)
            {
                //데이터가 있을경우
                if (selectobj != gameObject)
                {
                    //그 데이터가 제작결과 창일경우
                    if (selectobj.name == "CraftingResult")
                    {
                        //클릭 지점 기억
                            m_setDownPoint = selectobj.gameObject;
                     
                    }
                }
            }
        }

    }

    //클릭 종료
    public void OnPointerUp(PointerEventData p_eventData)
    {
        //선택된게 아이템슬롯인지 체크
        bool selectSlot = false;
        //마우스포인트가 밖인지체크
        bool outPoint = true;

        //게임오브젝트 정보받기
        foreach (GameObject selectObj in p_eventData.hovered)
        {
            //선택된 오브젝트가 자기자신인지 체크
            if (selectObj != gameObject)
            {
                if (m_itemData == null)
                    return;

                if (selectObj.GetComponent<RectTransform>())
                {
                    //창안에서 클릭end발생
                    outPoint = false;
                }
                    


                //해당 스크립트를 가지고있는지 확인
                if (selectObj.GetComponent<ItemSlotControl>())
                {
                    //필요 변수 선언
                    ItemSlotControl itemSlot = selectObj.GetComponent<ItemSlotControl>();
                    //서로 바꿔줄 임시 변수 선언
                    ItemData prevItem = m_itemData;
                    int prevItemNum = m_itemNum;

                    //좌클릭이 입력 끝났는지 확인
                    if (p_eventData.button == PointerEventData.InputButton.Left)
                    {
                        //마우스 클릭 끝난곳의 슬롯 타입 확인 
                        //슬롯타입이 모든 아이템타입이 들어갈수있는 곳이면 실행
                        if(itemSlot.m_SlotType==E_ItemType.Normal)
                        {

                            //데이터가 들어있고 바뀔 수 있는 아이템인지 확인
                            if (m_itemData != null && itemSlot.m_itemData != null && m_itemData.itemType == E_ItemType.Normal)
                            {
                                //같은 아이템 코드라면
                                if (m_itemData.itemcode == itemSlot.m_itemData.itemcode)
                                {
                                    //두개의 아이템 갯수 합을 확인
                                    int sumdata = m_itemNum + itemSlot.m_itemNum;
                                    //위치가 바뀔 아이템의 갯수가 64개 일경우
                                    if (itemSlot.m_itemNum == 64)
                                    {
                                        //아이템 스왑(서로 갯수와 아이템정보 바꿈)
                                        m_itemNum = itemSlot.m_itemNum;
                                        itemSlot.m_itemNum = prevItemNum;
                                        m_itemData = itemSlot.m_itemData;
                                        itemSlot.m_itemData = prevItem;
                                    }
                                    //합 갯수가 64개이하일 경우
                                    else if (sumdata <= 64)
                                    {
                                        //데이터를 한쪽으로 합친다
                                        itemSlot.m_itemNum += m_itemNum;
                                        m_itemNum = 0;
                                        m_itemData = null;
                                        itemSlot.m_itemData = prevItem;
                                    }
                                    //합 갯수가 최대갯수를 넘을 경우
                                    else if (sumdata > 64)
                                    {
                                        //최대치만 합치고 나머지는 그자리 그대로
                                        m_itemNum = m_itemNum + itemSlot.m_itemNum - 64;
                                        itemSlot.m_itemNum = 64;
                                        m_itemData = itemSlot.m_itemData;
                                        itemSlot.m_itemData = prevItem;
                                    }
                                }
                                else //다른아이템 스왑
                                {
                                    m_itemNum = itemSlot.m_itemNum;
                                    itemSlot.m_itemNum = prevItemNum;
                                    m_itemData = itemSlot.m_itemData;
                                    itemSlot.m_itemData = prevItem;
                                }
                            }
                            else
                            {
                                //반으로 나누기
                                if (Input.GetKey(KeyCode.LeftShift))
                                {
                                    //짝수 일 경우
                                    if (m_itemNum % 2 == 0)
                                    {
                                        m_itemNum = m_itemNum / 2;
                                        itemSlot.m_itemNum = prevItemNum / 2;
                                        itemSlot.m_itemData = prevItem;
                                    }
                                    //홀수 경우
                                    else
                                    {
                                        m_itemNum = (m_itemNum - 1) / 2;
                                        itemSlot.m_itemNum = (prevItemNum + 1) / 2;
                                        itemSlot.m_itemData = prevItem;
                                    }
                                }
                                else
                                {
                                    //데이터 스왑
                                    m_itemNum = itemSlot.m_itemNum;
                                    itemSlot.m_itemNum = prevItemNum;
                                    m_itemData = itemSlot.m_itemData;
                                    itemSlot.m_itemData = prevItem;
                                }
                            }
                        }
                        else
                        {
                            
                            //아이템슬롯타입이 노말이 아니였을 경우 해당 타입과 아이템타입이 같아야 실행
                            if(itemSlot.m_SlotType==m_itemData.itemType)
                            {
                                m_itemNum = itemSlot.m_itemNum;
                                itemSlot.m_itemNum = prevItemNum;
                                m_itemData = itemSlot.m_itemData;
                                itemSlot.m_itemData = prevItem;
                            }
                        }


                    }
                    //슬롯 업데이트
                    itemSlot.m_itemTransform.anchoredPosition = Vector3.zero;
                    itemSlot.UpdateSlotData();
                    UpdateSlotData();

                    //선택된곳이 아이템 슬롯이다라고 알려줌
                    selectSlot = true;
                }

            }
        }

        //아이템슬롯이 아닌곳에 클릭끝
        if (!selectSlot && outPoint)
        {
            if(m_itemData!=null)
            {
                //아이템 드랍 실행
                //아이템 데이터 ,갯수 설정 및 오브젝트 복사생성 + 오브젝트 활성화
                m_DropObject.GetComponent<Item>().m_itemData = m_itemData;
                m_DropObject.GetComponent<Item>().m_ItemNum = m_itemNum;
                Instantiate(m_DropObject, PlayerControl.GetInstance.transform.position, Quaternion.identity).SetActive(true);

                //슬롯에 저장된 데이터 삭제 및 슬롯 업데이트
                m_itemData = null;
                m_itemNum = 0;
                UpdateSlotData();
                m_itemTransform.anchoredPosition = Vector3.zero;
            }

        }
        else
        {
            //제작 완료
            if (m_setDownPoint != null) //눌린곳이 제작완료 였을 경우
            {
                //제작완료 처리
                m_recipeManager = m_setDownPoint.transform.parent.GetChild(0).GetComponent<RecipeManager>();
                m_recipeManager.CraftingComplete();
                m_setDownPoint = null;
            }
            else
            {
                UpdateSlotData();
                m_itemTransform.anchoredPosition = Vector3.zero;
            }
        }
        //다시 클릭가능 상태로 변경
        m_canvasGroup.blocksRaycasts = true;
    }

    //드레그중 발생
    public void OnDrag(PointerEventData p_eventData)
    {
        //좌클릭 드래그
        if (p_eventData.button == PointerEventData.InputButton.Left)
        {
            if (m_itemData != null)
            {
                //이동
                m_itemTransform.anchoredPosition += p_eventData.delta / m_canvas.scaleFactor;
            }
        }
    }

    //마우스 포인터가 아이템슬롯에 올라왔을 경우 실행
    public void OnPointerEnter(PointerEventData eventData)
    {
        if(m_itemData != null) //아이템 데이터가 있을경우
        {
            //해당 아이템의 툴팁 생성 데이터와 위치 아이템 갯수 전달
            m_itemToolTip.GetComponent<ItemToolTip>().GetToolTip(m_itemData, this.transform.position, m_itemNum);
        }

    }

    public void OnPointerExit(PointerEventData eventData)
    {
        //해당 슬롯 벗어나면 숨김
        m_itemToolTip.GetComponent<ItemToolTip>().HideToolTip();
    }
}
