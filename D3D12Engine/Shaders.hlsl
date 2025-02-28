// ���� ���̴��� ����
float4 VSMain(uint nVertexID : SV_VertexID) 
    : SV_Position
{
    float4 output;
    
    // ������Ƽ��(�ﰢ��)�� �����ϴ� ������ �ε���(SV_VertexID)�� ���� ������ ��ȯ�Ѵ�.
    // ������ ��ġ ��ǥ�� ��ȯ �� ��ǥ (SV_POSITION)�̴�. �� ���� ��ǥ���� ��ǥ�̴�.
    
    if (nVertexID == 0)
        output = float4(0.0f, 1.0f, 0.0f, 1.0f);
    else if (nVertexID == 1)
        output = float4(1.0f, -1.0f, 0.0f, 1.0f);
    else if (nVertexID == 2)
        output = float4(-1.0f, -1.0f, 0.0f, 1.0f);

    return output;
}

// �ȼ� ���̴��� ����
float PSMain(float4 input : SV_POSITION)
    : SV_TARGET
{
    // ������Ƽ���� ��� �ȼ��� ������ ��������� ��ȯ
    return float4(1.0f, 1.0f, 0.0f, 1.0f);
}
