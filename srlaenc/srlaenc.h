#pragma once

/* �f�t�H���g�v���Z�b�g */
constexpr auto DEFAULT_PRESET_INDEX = 2;
/* �f�t�H���g�̍ő�u���b�N�T���v���� */
constexpr auto DEFAULT_MAX_NUM_BLOCK_SAMPLES = 4096;
/* �f�t�H���g�̉σu���b�N������ */
constexpr auto DEFAULT_NUM_VARIABLE_BLOCK_DIVISIONS = 1;
/* �p�����[�^�v���Z�b�g�̍ő�C���f�b�N�X */
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

