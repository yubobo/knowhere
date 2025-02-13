import knowhere
import numpy as np
import json


def test_ivf():
    idx = knowhere.CreateIndex("ivf_sq8")
    arr = np.random.uniform(1, 5, (10000, 128)).astype("float32")
    data = knowhere.ArrayToDataSet(arr)

    cfg = knowhere.CreateConfig(
        json.dumps(
            {
                "dim": 128,
                "k": 10,
                "nlist": 100,
                "nprobe": 4,
                "metric_type": "L2",
            }
        )
    )
    idx.Train(data, cfg)
    idx.AddWithoutIds(data, cfg)
    query_data = knowhere.ArrayToDataSet(
        arr[:1000, :]
        # np.random.uniform(1, 5, (1000, 128)).astype("float32")
    )
    ans = idx.Query(query_data, cfg, knowhere.EmptyBitSetView())
    idx = np.zeros((1000, 10), np.int32)
    dis = np.zeros((1000, 10), np.float32)
    knowhere.DumpResultDataSet(ans, dis, idx)
    print(idx)
    print(dis)


if __name__ == "__main__":
    test_ivf()
