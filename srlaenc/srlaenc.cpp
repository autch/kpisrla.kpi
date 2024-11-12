#define _CRT_SECURE_NO_WARNINGS
#include <cstdio>
#include <cstdlib>
#include <functional>

#include <Windows.h>
#include <mmreg.h>

#include "srlaenc.h"
#include "WaveReader.h"
#include "srla_encoder.h"
#include "Deinterleaver.h"

// parse_waveformat.cpp
int parse_waveformat(WaveReader& wav, pcm_params* params);

int usage(char* argv0)
{
	printf("Usage: %s [options] input.wav output.srla\n", argv0);
	printf("Options:\n");
	printf("  -e           Do nothing; just for compatibility with srla\n");
	printf("  -m <preset>  Encoder preset number (0-4), default %d\n", DEFAULT_PRESET_INDEX);
	printf("  -B <samples> Maximum samples per block, default %d\n", DEFAULT_MAX_NUM_BLOCK_SAMPLES);
	printf("  -V <divs>    Variable block divisions, default %d\n", DEFAULT_NUM_VARIABLE_BLOCK_DIVISIONS);
	return 1;
}

int parse_args(int ac, char** av, options* options)
{
	char* argv0 = *av;

	while(*++av)
	{
		char* arg = *av;

		if (arg[0] == '-')
		{
			switch(arg[1])
			{
			case 'e':
				// do nothing, just for compatibility with srla command
				break;
			case 'd':
				printf("Decoding not supported\n");
				return usage(argv0);
			case 'm':
				if (!*(av + 1))
				{
					fprintf(stderr, "%s: Option -m requires an argument\n", argv0);
					return usage(argv0);
				}
				options->encoder_preset_num = atoi(*++av);
				break;
			case 'B':
				if (!*(av + 1))
				{
					fprintf(stderr, "%s: Option -B requires an argument\n", argv0);
					return usage(argv0);
				}
				options->max_samples_per_block = atoi(*++av);
				break;
			case 'V':
				if (!*(av + 1))
				{
					fprintf(stderr, "%s: Option -V requires an argument\n", argv0);
					return usage(argv0);
				}
				options->variable_block_divisions = atoi(*++av);
				break;
			default:
				fprintf(stderr, "%s: Unknown option: %s\n", argv0, arg);
			case 'h':
				return usage(argv0);
			}
		}
		else
		{
			if (!options->input_filename)
				options->input_filename = arg;
			else if (!options->output_filename)
				options->output_filename = arg;
			else
				return usage(argv0);
		}
	}
	if (options->input_filename == nullptr || options->output_filename == nullptr)
		return usage(argv0);

	return 0;
}

int main(int ac, char** av)
{
	struct options options {};
	int ret = parse_args(ac, av, &options);

	if(ret != 0)
		return ret;


	struct SRLAEncoder *encoder;
    struct SRLAEncoderConfig config;
    struct SRLAEncodeParameter parameter;
    uint32_t encoded_data_size;
	typedef SRLAApiResult (*fn_encode_function)(struct SRLAEncoder* encoder,
	                                            const int32_t* const* input, uint32_t num_samples,
	                                            uint8_t * data, uint32_t data_size, uint32_t * output_size);

    /* WAVファイルオープン */
    WaveReader wave;
	if(!wave.Open(options.input_filename))
	{
        fprintf(stderr, "Failed to open %s. \n", options.input_filename);
        return 1;
    }
	pcm_params pcm_params;
	if (parse_waveformat(wave, &pcm_params) != 0)
	{
		return 1;
	}
	MMCKINFO ckData;
	if(!wave.FindDataChunk(&ckData))
	{
		fprintf(stderr, "Failed to find data chunk\n");
		return 1;
	}

	FILE* out_fp = fopen(options.output_filename, "wb");
	if (!out_fp)
	{
		fprintf(stderr, "Failed to open %s for writing\n", options.output_filename);
		return 1;
	}

	/* エンコーダコンフィグの初期化 */
	memset(&config, 0, sizeof(config));

	/* エンコーダ作成 */
    config.max_num_channels = SRLA_MAX_NUM_CHANNELS;
    config.min_num_samples_per_block = options.max_samples_per_block >> options.variable_block_divisions;
    config.max_num_samples_per_block = options.max_samples_per_block;
    config.max_num_parameters = SRLA_MAX_COEFFICIENT_ORDER;
    if ((encoder = SRLAEncoder_Create(&config, nullptr, 0)) == nullptr) {
        fprintf(stderr, "Failed to create encoder handle. \n");
        return 1;
    }

    /* エンコードパラメータセット */
    parameter.num_channels = pcm_params.channels;
    parameter.bits_per_sample = pcm_params.bits_per_sample;
    parameter.sampling_rate = pcm_params.samples_per_sec;
    parameter.min_num_samples_per_block = options.max_samples_per_block >> options.variable_block_divisions;
    parameter.max_num_samples_per_block = options.max_samples_per_block;

	/* プリセットの反映 */
    parameter.preset = static_cast<uint8_t>(options.encoder_preset_num);
    if ((ret = SRLAEncoder_SetEncodeParameter(encoder, &parameter)) != SRLA_APIRESULT_OK) {
        fprintf(stderr, "Failed to set encode parameter: %d \n", ret);
        return 1;
    }

	uint32_t num_samples = ckData.cksize / pcm_params.block_align;

	uint32_t buffer_size = (options.max_samples_per_block * pcm_params.channels * (pcm_params.bits_per_sample / 8) + SRLA_HEADER_SIZE) * 2;
	uint32_t input_buffer_size = (static_cast<size_t>(options.max_samples_per_block) * pcm_params.channels * sizeof(int32_t) + SRLA_HEADER_SIZE) * 2;
	auto buffer = std::make_unique<uint8_t[]>(buffer_size);
	auto input_buffer = std::make_unique<uint8_t[]>(input_buffer_size);

    /* エンコード関数の選択 */
    fn_encode_function encode_function = (options.variable_block_divisions == 0) ? SRLAEncoder_EncodeBlock : SRLAEncoder_EncodeOptimalPartitionedBlock;

    /* エンコード実行 */
    {
        uint8_t *data_pos = buffer.get();
        uint32_t write_offset = 0;
        struct SRLAHeader header;
		Deinterleaver di(parameter.num_channels, options.max_samples_per_block);

        /* ヘッダエンコード */
        header.num_channels = parameter.num_channels;
        header.num_samples = num_samples;
        header.sampling_rate = parameter.sampling_rate;
        header.bits_per_sample = parameter.bits_per_sample;
        header.preset = parameter.preset;
        header.max_num_samples_per_block = parameter.max_num_samples_per_block;
        if ((ret = SRLAEncoder_EncodeHeader(&header, data_pos, buffer_size))
                != SRLA_APIRESULT_OK) {
            fprintf(stderr, "Failed to encode header! ret:%d \n", ret);
            return 1;
        }
		fwrite(data_pos, 1, SRLA_HEADER_SIZE, out_fp);

		Deinterleaver::fnReadSample fn = di.get_accessor(header.bits_per_sample);

		auto buffer_ptrs = std::make_unique<int32_t*[]>(parameter.num_channels);
        /* ブロックを時系列順にエンコード */
        uint32_t progress = 0;
        while (progress < num_samples) {
            uint32_t write_size;
            /* エンコードサンプル数の確定 */
            const uint32_t num_encode_samples = min(parameter.max_num_samples_per_block, num_samples - progress);

			size_t bytes_read = wave.Read(input_buffer.get(), static_cast<LONG>(num_encode_samples) * pcm_params.block_align);
			if (bytes_read < static_cast<size_t>(num_encode_samples) * pcm_params.block_align)
			{
				fprintf(stderr, "Failed to read %d bytes\n", num_encode_samples * pcm_params.block_align);
				return 1;
			}

        	size_t samples_read = bytes_read / pcm_params.block_align;
			uint32_t samples_to_encode = samples_read;

			di.deinterleave(input_buffer.get(), samples_to_encode,fn, buffer_ptrs.get());

            /* ブロックエンコード */
            if ((ret = encode_function(encoder,
                    buffer_ptrs.get(), samples_to_encode,
                    data_pos, buffer_size, &write_size)) != SRLA_APIRESULT_OK) {
                fprintf(stderr, "Failed to encode! ret:%d \n", ret);
                return 1;
            }
			fwrite(data_pos, 1, write_size, out_fp);

            /* 進捗更新 */
            progress += num_encode_samples;
			write_offset += write_size;

            /* 進捗表示 */
            printf("progress... %5.2f%% \r", (double)((progress * 100.0) / num_samples));
            fflush(stdout);
        }

        /* 書き出しサイズ取得 */
        encoded_data_size = write_offset;
    }

    /* 圧縮結果サマリの表示 */
    printf("finished: %lu -> %d (%6.2f %%) \n",
            ckData.cksize, encoded_data_size, (double)((100.0 * encoded_data_size) / static_cast<double>(ckData.cksize)));

    /* リソース破棄 */
    fclose(out_fp);
    SRLAEncoder_Destroy(encoder);

	return 0;
}
