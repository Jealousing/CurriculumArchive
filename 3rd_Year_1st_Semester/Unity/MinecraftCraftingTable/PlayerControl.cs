using System.Collections;
using System.Collections.Generic;
using UnityEngine;

//플레이어 컨트롤 
public class PlayerControl : MonoBehaviour
{
    //싱글톤화 작업
    private static PlayerControl m_Instance = null;
    
    //인벤토리, 크래프팅테이블 , 툴팁 관련 UI컨트롤
    public GameObject m_Inventory;
    public GameObject m_CraftingTable;
    public GameObject m_ToolTip;

    //이동관련 
    private Vector3 m_Movement;
    private Rigidbody m_Rigidbody;
    private float m_HorizontalMove;
    private float m_VerticalMove;

    //속도관련
    public float m_MoveSpeed = 5f;
    public float m_RotateSpeed = 10f;
    public float m_JumpPower = 5f;

    //점프여부
    bool m_IsJump;

    //키막기
    bool m_MoveKeyBlock;
    bool m_UiKeyBlock;

    //싱글톤화 작업
    private void Awake()
    {
        if (m_Instance == null)
        {
            m_Instance = this;
            DontDestroyOnLoad(this.gameObject);
        }
        else
        {
            Destroy(this.gameObject);
        }
    }

    void Start()
    {
        m_Rigidbody = GetComponent<Rigidbody>();
        m_IsJump = false;
        m_MoveKeyBlock = false;
        m_UiKeyBlock = false;
    }

    //싱글톤
    public static PlayerControl GetInstance
    {
        get
        {
            if (m_Instance == null)
            {
                return null;
            }
            return m_Instance;
        }
    }

    void Update()
    {
        //키입력받아옴
        InputKey();
    }
    void FixedUpdate()
    {
        //물리작용
        Moveing();
        Turn();
    }

    //키입력
    void InputKey()
    {
        //esc로 ui 종료
        if (Input.GetKeyDown(KeyCode.Escape))
        {
            if (m_Inventory.gameObject.activeSelf == true)
            {
                //활성화된 ui들 비활성화
                GetComponent<InventorySystem>().m_FavoriteInventory.SetActive(false);
                GetComponent<InventorySystem>().m_InventorySystem.SetActive(false);
                GetComponent<InventorySystem>().m_Inventory.SetActive(false);
                m_ToolTip.GetComponent<ItemToolTip>().HideToolTip();
                m_Inventory.gameObject.SetActive(false);
                m_MoveKeyBlock = false; //블럭 해제
            }

            if (m_CraftingTable.gameObject.activeSelf == true)
            {
                //활성화된 ui들 비활성화
                GetComponent<InventorySystem>().m_FavoriteInventory.SetActive(false);
                GetComponent<InventorySystem>().m_InventorySystem.SetActive(false);
                GetComponent<InventorySystem>().m_Inventory.SetActive(false);
                m_ToolTip.GetComponent<ItemToolTip>().HideToolTip();
                m_CraftingTable.gameObject.SetActive(false);
                m_MoveKeyBlock = false;//블럭 해제
            }
            m_UiKeyBlock = false;
        }

        
        if (!m_MoveKeyBlock)
        {
            //움직임 관련 키입력 받아옴
            MoveKey();
        }
        if(!m_UiKeyBlock)
        {
            //ui조작 키
            UiKey();
        }
        else
        {
            //ui종료키
            UnUiKey();
        }
        
    }


    //움직임  관련 키 입력 
    void MoveKey()
    {
        //이동받아옴
        m_HorizontalMove = Input.GetAxisRaw("Horizontal");
        m_VerticalMove = Input.GetAxisRaw("Vertical");

        //처음 점프버튼이 눌렸을 경우
        if (Input.GetKeyDown(KeyCode.Space))
        {
            //점프가 진행중이 아니면 실행
            if (!m_IsJump)
            {
                //점프
                Jump();
            }
        }
        //점프 중에 스페이스바를 때거나 
        if (Input.GetKeyUp(KeyCode.Space) && m_Rigidbody.velocity.y > 0)
        {
            m_IsJump = true;
            m_Rigidbody.velocity = m_Rigidbody.velocity * 0.5f;
        }
    }

    //ui종료
    void UnUiKey()
    {
        //가방 인벤토리 종료
        if (Input.GetKeyDown(KeyCode.E))
        {
            if (m_Inventory.gameObject.activeSelf == true)
            {
                //비활성화
                GetComponent<InventorySystem>().m_FavoriteInventory.SetActive(false);
                GetComponent<InventorySystem>().m_InventorySystem.SetActive(false);
                GetComponent<InventorySystem>().m_Inventory.SetActive(false);
                m_ToolTip.GetComponent<ItemToolTip>().HideToolTip();
                m_Inventory.gameObject.SetActive(false);
                //키블럭해제
                m_UiKeyBlock = false;
                m_MoveKeyBlock = false;
                ResetVelocity();
            }
        }
    }

    //ui 조작키
    void UiKey()
    {

        if(Input.GetKeyDown(KeyCode.E))
        {
            if(m_Inventory.gameObject.activeSelf==false)
            {
                //관련 ui 활성화
                GetComponent<InventorySystem>().m_FavoriteInventory.SetActive(true);
                GetComponent<InventorySystem>().m_InventorySystem.SetActive(true);
                GetComponent<InventorySystem>().m_Inventory.SetActive(true);
                m_Inventory.gameObject.SetActive(true);
                //키블럭
                m_UiKeyBlock = true;
                m_MoveKeyBlock = true;
                //이동초기화(멈춤)
                ResetVelocity();
            }
        }

        if(Input.GetMouseButtonDown(1))
        {
            float distance = 5f;
            RaycastHit hit;
            //레이발사
            Debug.DrawRay(transform.position, transform.forward * distance, Color.red, 0.5f);
            //레이가 맞으면
            if(Physics.Raycast(transform.position,transform.forward,out hit,distance))
            {
                //레이의 테그검색
                switch (hit.transform.tag)
                {
                    //태그가 크레프팅테이블이면
                    case "CraftingTable":
                        //관련 ui활성화
                        m_CraftingTable.gameObject.SetActive(true);
                        GetComponent<InventorySystem>().m_FavoriteInventory.SetActive(true);
                        GetComponent<InventorySystem>().m_InventorySystem.SetActive(true);
                        GetComponent<InventorySystem>().m_Inventory.SetActive(true);
                        //키블럭
                        m_UiKeyBlock = true;
                        m_MoveKeyBlock = true;
                        //움직임 멈춤
                        ResetVelocity();
                        break;
                }

            }

        }

    }

    //이동
    void Moveing()
    {
        m_Movement.Set(m_HorizontalMove, 0, m_VerticalMove);
        m_Movement = m_Movement.normalized * m_MoveSpeed * Time.deltaTime;

        m_Rigidbody.MovePosition(transform.position + m_Movement);

    }
    //회전
    void Turn()
    {
        if (m_HorizontalMove == 0 && m_VerticalMove == 0)
            return;
        //회전처리
        Quaternion newRotation = Quaternion.LookRotation(m_Movement);
        m_Rigidbody.rotation = Quaternion.Slerp(m_Rigidbody.rotation, newRotation, m_RotateSpeed * Time.deltaTime);
    }
    void Jump() //점프
    {

        //점프중이면 점프못함
        if (m_IsJump)
            return;
        if (m_Rigidbody.velocity.y < 0)
        {
            m_IsJump=true;
        }

        //문제없이 여기까지 왔으면 점프처리
        m_Rigidbody.velocity = Vector3.zero;
        //입력받은 점프의 힘만큼 점프
        m_Rigidbody.AddForce(Vector3.up * m_JumpPower, ForceMode.Impulse);

        

    }

    //velocity 초기화
    void ResetVelocity()
    {
        m_Rigidbody.velocity = Vector3.zero;
        m_HorizontalMove = 0;
        m_VerticalMove = 0;
    }

    //바닥 충돌시 점프활성화
    private void OnCollisionEnter(Collision collision)
    {
        if (collision.gameObject.tag == "Plane")
        {
            m_IsJump = false;
        }
    }


}
