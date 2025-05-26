def make_pad_mask(lengths: Tensor,
    max_len: int=0) -> Tensor:
  batch_size = torch.size(lengths, 0)
  if torch.gt(max_len, 0):
    max_len0 = max_len
  else:
    max_len0 = torch.item(torch.max(lengths))
  seq_range = torch.arange(0, max_len0, dtype=4, layout=None, device=ops.prim.device(lengths))
  seq_range_expand = torch.expand(torch.unsqueeze(seq_range, 0), [batch_size, int(max_len0)])
  seq_length_expand = torch.unsqueeze(lengths, -1)
  mask = torch.ge(seq_range_expand, seq_length_expand)
  return mask
def add_optional_chunk_mask(xs: Tensor,
    masks: Tensor,
    use_dynamic_chunk: bool,
    use_dynamic_left_chunk: bool,
    decoding_chunk_size: int,
    static_chunk_size: int,
    num_decoding_left_chunks: int) -> Tensor:
  _0 = __torch__.wenet.utils.mask.subsequent_chunk_mask
  if use_dynamic_chunk:
    max_len = torch.size(xs, 1)
    if torch.lt(decoding_chunk_size, 0):
      chunk_size, num_left_chunks = max_len, -1
    else:
      if torch.gt(decoding_chunk_size, 0):
        chunk_size0, num_left_chunks0 = decoding_chunk_size, num_decoding_left_chunks
      else:
        chunk_size1 = torch.item(torch.randint(1, max_len, [1]))
        _1 = torch.gt(chunk_size1, torch.floordiv(max_len, 2))
        if _1:
          chunk_size2, num_left_chunks1 = max_len, -1
        else:
          _2 = torch.remainder(chunk_size1, 25)
          chunk_size3 = torch.add(_2, 1)
          if use_dynamic_left_chunk:
            max_left_chunks = torch.floordiv(torch.sub(max_len, 1), chunk_size3)
            _3 = torch.randint(0, int(max_left_chunks), [1])
            num_left_chunks3 = torch.item(_3)
            num_left_chunks2 = int(num_left_chunks3)
          else:
            num_left_chunks2 = -1
          chunk_size2, num_left_chunks1 = chunk_size3, num_left_chunks2
        chunk_size0, num_left_chunks0 = chunk_size2, num_left_chunks1
      chunk_size, num_left_chunks = chunk_size0, num_left_chunks0
    _4 = torch.size(xs, 1)
    _5 = ops.prim.device(xs)
    chunk_masks0 = _0(_4, int(chunk_size), num_left_chunks, _5, )
    chunk_masks1 = torch.unsqueeze(chunk_masks0, 0)
    chunk_masks = torch.__and__(masks, chunk_masks1)
  else:
    if torch.gt(static_chunk_size, 0):
      chunk_masks3 = _0(torch.size(xs, 1), static_chunk_size, num_decoding_left_chunks, ops.prim.device(xs), )
      chunk_masks4 = torch.unsqueeze(chunk_masks3, 0)
      chunk_masks5 = torch.__and__(masks, chunk_masks4)
      chunk_masks2 = chunk_masks5
    else:
      chunk_masks2 = masks
    chunk_masks = chunk_masks2
  return chunk_masks
def subsequent_mask(size: int,
    device: Device=torch.device("cpu")) -> Tensor:
  arange = torch.arange(size, dtype=None, layout=None, device=device)
  mask = torch.expand(arange, [size, size])
  arange0 = torch.unsqueeze(arange, -1)
  return torch.le(mask, arange0)
def subsequent_chunk_mask(size: int,
    chunk_size: int,
    num_left_chunks: int=-1,
    device: Device=torch.device("cpu")) -> Tensor:
  ret = torch.zeros([size, size], dtype=11, layout=None, device=device)
  for i in range(size):
    if torch.lt(num_left_chunks, 0):
      start = 0
    else:
      _6 = torch.sub(torch.floordiv(i, chunk_size), num_left_chunks)
      start0 = ops.prim.max(torch.mul(_6, chunk_size), 0)
      start = start0
    _7 = torch.add(torch.floordiv(i, chunk_size), 1)
    ending = ops.prim.min(torch.mul(_7, chunk_size), size)
    _8 = torch.slice(torch.select(ret, 0, i), 0, start, ending)
    _9 = torch.tensor(True, dtype=ops.prim.dtype(_8), device=ops.prim.device(_8))
    _10 = torch.copy_(_8, _9)
  return ret
