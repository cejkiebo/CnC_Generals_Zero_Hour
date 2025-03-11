﻿// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.
#pragma once

namespace D3D9on12
{
    class Device; // Forward declaration
    struct D3D12VertexShader;
    struct D3D12PixelShader;
    struct D3D12GeometryShader;
    class Shader;

#pragma pack(push, 1)
    struct PipelineStateKey
    {
        // Hash in the constructor so that his key can be used several times efficiently
        PipelineStateKey(D3D12_GRAPHICS_PIPELINE_STATE_DESC& desc, D3D12VertexShader* pVS, D3D12PixelShader* pPS, D3D12GeometryShader* pGS) :
            m_desc(desc, pVS, pPS, pGS)
        {
        };

        bool operator==(const PipelineStateKey &key) const
        {
            return memcmp(&m_desc, &key.m_desc, sizeof(key.m_desc)) == 0;
        }

        // Hashing and comparison of PSO descriptors is slow and expensive. Optimize by bitpacking only the values that we care about
        // for D3D9 this way we can hash and compare 64 bytes vs 572
        struct D3D9on12PipelineStateDesc
        {
            D3D9on12PipelineStateDesc(D3D12_GRAPHICS_PIPELINE_STATE_DESC& desc, D3D12VertexShader* pVS, D3D12PixelShader* pPS, D3D12GeometryShader* pGS) :
                m_pVS(pVS), m_pPS(pPS), m_pGS(pGS), m_compressedData(desc)
            {
                //Values which can be compressed further
                SampleMask = desc.SampleMask;
                DepthBias = desc.RasterizerState.DepthBias;
                DepthBiasClamp = desc.RasterizerState.DepthBiasClamp;
                SlopeScaledDepthBias = desc.RasterizerState.SlopeScaledDepthBias;
                StencilReadMask = desc.DepthStencilState.StencilReadMask;
                StencilWriteMask = desc.DepthStencilState.StencilWriteMask;
            }

            D3D12VertexShader* m_pVS;// Vertex Shaders and Input layouts are tied together so only need the VS
            D3D12PixelShader* m_pPS;
            D3D12GeometryShader* m_pGS;
            UINT SampleMask;
            INT DepthBias;
            FLOAT DepthBiasClamp;
            FLOAT SlopeScaledDepthBias;
            UINT8 StencilReadMask;
            UINT8 StencilWriteMask;

#define BitPackedBlendDesc(number) \
                UINT BlendDescBlendEnable##number            : 1; \
                UINT BlendDescLogicOpEnable##number          : 1; \
                UINT BlendDescSrcBlend##number               : 5; \
                UINT BlendDescDestBlend##number              : 5; \
                UINT BlendDescBlendOp##number                : 3; \
                UINT BlendDescSrcBlendAlpha##number          : 5; \
                UINT BlendDescDestBlendAlpha##number         : 5; \
                UINT BlendDescBlendOpAlpha##number           : 3; \
                UINT BlendDescLogicOp##number                : 4; \
                UINT BlendDescRenderTargetWriteMask##number  : 8; 

            struct CompressedData
            {
                static const UINT m_RemderTargetFormatBits = 5;
                static const UINT m_SampleCountBits = 4;

                CompressedData(D3D12_GRAPHICS_PIPELINE_STATE_DESC& desc);
                FORCEINLINE UINT8 CompressDepthFormat(DXGI_FORMAT);
                FORCEINLINE UINT8 CompressRenderTargetFormat(DXGI_FORMAT);

                UINT PrimitiveTopologyType      : 3;
                // Render Targets
                UINT NumRenderTargets           : 2;
                UINT RenderTargetFormat0        : m_RemderTargetFormatBits;//We expose < 32 RT formats so 5 bits will do, if caps change so might this!
                UINT RenderTargetFormat1        : m_RemderTargetFormatBits;
                UINT RenderTargetFormat2        : m_RemderTargetFormatBits;
                UINT RenderTargetFormat3        : m_RemderTargetFormatBits;
                UINT DepthStencilFormat         : 3;
                // Sample Desc
                UINT SampleCount                : m_SampleCountBits;
                UINT SampleQuality              : m_SampleCountBits;
                // RasterState
                UINT FillMode                   : 1;
                UINT CullMode                   : 2;
                UINT FrontCounterClockwise      : 1;
                UINT DepthClipEnable            : 1;
                UINT MultisampleEnable          : 1;
                UINT AntialiasedLineEnable      : 1;
                UINT ForcedSampleCount          : m_SampleCountBits;
                // Depth Desc
                UINT DepthEnable                : 1;
                UINT DepthWriteMask             : 1;
                UINT DepthFunc                  : 3;
                UINT StencilEnable              : 1;
                UINT FrontStencilFailOp         : 3;
                UINT FrontStencilDepthFailOp    : 3;
                UINT FrontStencilPassOp         : 3;
                UINT FrontStencilFunc           : 3;
                UINT BackStencilFailOp          : 3; 
                UINT BackStencilDepthFailOp     : 3;
                UINT BackStencilPassOp          : 3;
                UINT BackStencilFunc            : 3;

                BitPackedBlendDesc(0);
                BitPackedBlendDesc(1);
                BitPackedBlendDesc(2);
                BitPackedBlendDesc(3);
            };

            CompressedData m_compressedData;

        };

        D3D9on12PipelineStateDesc m_desc;
    };
#pragma pack(pop)
}

namespace std
{
    template<>
    struct hash<D3D9on12::PipelineStateKey>
    {

        size_t operator()(D3D9on12::PipelineStateKey const& key) const
        {
            return size_t(D3D9on12::HashData(&key.m_desc, sizeof(key.m_desc)).m_data);
        }
    };
}

namespace D3D9on12
{
    struct PipelineStateCacheEntry
    {
        PipelineStateCacheEntry(const PipelineStateKey& key) : m_key(key) {}

        std::unique_ptr<D3D12TranslationLayer::PipelineState> m_pPipelineState;
        UINT64 m_timestamp;
        std::list<std::shared_ptr<PipelineStateCacheEntry>>::iterator m_accessOrderPos;
        PipelineStateKey m_key;
    };

    struct PipelineStateCacheImpl
    {
        std::list<std::shared_ptr<PipelineStateCacheEntry>> m_accessOrder;
        std::unordered_map<PipelineStateKey, std::shared_ptr<PipelineStateCacheEntry>> m_map;
    };

    struct PipelineStateCacheKeyComponent
    {
    public:
        PipelineStateCacheKeyComponent(PipelineStateCacheImpl &cache) : m_cache(cache) {}
        ~PipelineStateCacheKeyComponent() 
        { 
            for (auto &key : m_pPSOKeys)
            {
                auto it = m_cache.m_map.find(key);

                if (it != m_cache.m_map.end())
                {
                    assert(m_cache.m_map.size() == m_cache.m_accessOrder.size());
                    assert(it->second.get() == it->second->m_accessOrderPos->get());
                    m_cache.m_accessOrder.erase(it->second->m_accessOrderPos);
                    m_cache.m_map.erase(it);
                }
            }
        }

        // Move-only
        PipelineStateCacheKeyComponent() = default;
        PipelineStateCacheKeyComponent(PipelineStateCacheKeyComponent const&) = delete;
        PipelineStateCacheKeyComponent& operator=(PipelineStateCacheKeyComponent const&) = delete;
        PipelineStateCacheKeyComponent(PipelineStateCacheKeyComponent&&) = default;
        PipelineStateCacheKeyComponent& operator=(PipelineStateCacheKeyComponent&&) = default;

        void AddPSO(const PipelineStateKey &key)
        {
            m_pPSOKeys.insert(key);
        }

        void RemovePSO(const PipelineStateKey& key)
        {
            m_pPSOKeys.erase(key);
        }

    private:
        std::unordered_set<PipelineStateKey> m_pPSOKeys;
        PipelineStateCacheImpl &m_cache;
    };

    class PipelineStateCache
    {
    public:
        PipelineStateCache(Device &device) : 
            m_device(device) {}

        D3D12TranslationLayer::PipelineState * GetPipelineState(D3D12_GRAPHICS_PIPELINE_STATE_DESC &psoDesc, D3D12VertexShader* pVS, D3D12PixelShader* pPS, D3D12GeometryShader* pGS);
        
        PipelineStateCacheImpl &GetCache() { return m_cache; }
    private:
        void AddUses(Shader &ps, Shader &vs, PipelineStateKey key);
        void Trim();

        Device &m_device;
        PipelineStateCacheImpl m_cache;
    };

    struct ShaderKey
    {
        const byte *pBytecode;
        size_t size;
        WeakHash hash;

        struct Hasher
        {
            size_t operator()(ShaderKey const& key) const { return key.hash.m_data; }
        };
        bool operator==(ShaderKey const& o) const
        {
            return size == o.size &&
                (pBytecode == o.pBytecode ||
                    memcmp(pBytecode, o.pBytecode, size) == 0);
        }
    };

    template <typename TShader>
    struct ShaderDedupe
    {
        std::unordered_map<ShaderKey, std::unique_ptr<TShader>, ShaderKey::Hasher> map;
        TShader *GetOrCreate(Device& device, const byte *pBytecode, size_t size)
        {
            ShaderKey searchKey{ pBytecode, size, HashData(pBytecode, size) };
            auto iter = map.find(searchKey);
            if (iter != map.end())
            {
                iter->second->AddRef();
                return iter->second.get();
            }

            std::unique_ptr<TShader> shader(new TShader(device, *pBytecode, size, searchKey.hash));
            ShaderKey insertKey{ shader->GetLegacyByteCode().m_ptr, shader->GetLegacyByteCode().m_size, searchKey.hash };
            auto ret = map.emplace(insertKey, std::move(shader));
            assert(ret.second);
            return ret.first->second.get();
        }

        void Release(TShader *shader)
        {
            UINT refCount = shader->Release();
            if (refCount == 0)
            {
                ShaderKey searchKey{ shader->GetLegacyByteCode().m_ptr, shader->GetLegacyByteCode().m_size, shader->GetHashForLegacyByteCode() };
                [[maybe_unused]] size_t num_erased = map.erase(searchKey);
                assert(num_erased == 1);
            }
        }
    };

};
