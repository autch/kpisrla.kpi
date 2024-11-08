#pragma once

/* デフォルトプリセット */
constexpr auto DEFAULT_PRESET_INDEX = 2;
/* デフォルトの最大ブロックサンプル数 */
constexpr auto DEFAULT_MAX_NUM_BLOCK_SAMPLES = 4096;
/* デフォルトの可変ブロック分割数 */
constexpr auto DEFAULT_NUM_VARIABLE_BLOCK_DIVISIONS = 1;
/* パラメータプリセットの最大インデックス */
constexpr auto SRLA_MAX_PARAMETER_PRESETS_INDEX = 4;

struct options
{
	int encoder_preset_num{DEFAULT_PRESET_INDEX};
	int max_samples_per_block{DEFAULT_MAX_NUM_BLOCK_SAMPLES};
	int variable_block_divisions{DEFAULT_NUM_VARIABLE_BLOCK_DIVISIONS};

	char* input_filename{nullptr};
	char* output_filename{nullptr};
};

struct pcm_params
{
	WORD channels;
	DWORD samples_per_sec;
	WORD bits_per_sample;
	WORD block_align;
};

