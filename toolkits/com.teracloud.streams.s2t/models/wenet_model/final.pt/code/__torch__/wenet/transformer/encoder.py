class ConformerEncoder(Module):
  __parameters__ = []
  __buffers__ = []
  training : bool
  _is_full_backward_hook : Optional[bool]
  _output_size : int
  normalize_before : bool
  static_chunk_size : int
  use_dynamic_chunk : bool
  use_dynamic_left_chunk : bool
  global_cmvn : __torch__.wenet.transformer.cmvn.GlobalCMVN
  embed : __torch__.wenet.transformer.subsampling.Conv2dSubsampling6
  after_norm : __torch__.torch.nn.modules.normalization.LayerNorm
  encoders : __torch__.torch.nn.modules.container.ModuleList
  def forward(self: __torch__.wenet.transformer.encoder.ConformerEncoder,
    xs: Tensor,
    xs_lens: Tensor,
    decoding_chunk_size: int=0,
    num_decoding_left_chunks: int=-1) -> Tuple[Tensor, Tensor]:
    _0 = __torch__.wenet.utils.mask.make_pad_mask
    _1 = __torch__.wenet.utils.mask.add_optional_chunk_mask
    T = torch.size(xs, 1)
    _2 = torch.unsqueeze(_0(xs_lens, T, ), 1)
    masks = torch.bitwise_not(_2)
    xs0 = (self.global_cmvn).forward(xs, )
    _3 = (self.embed).forward(xs0, masks, 0, )
    xs1, pos_emb, masks0, = _3
    chunk_masks = _1(xs1, masks0, self.use_dynamic_chunk, self.use_dynamic_left_chunk, decoding_chunk_size, self.static_chunk_size, num_decoding_left_chunks, )
    _4 = self.encoders
    _5 = getattr(_4, "0")
    _6 = getattr(_4, "1")
    _7 = getattr(_4, "2")
    _8 = getattr(_4, "3")
    _9 = getattr(_4, "4")
    _10 = getattr(_4, "5")
    _11 = getattr(_4, "6")
    _12 = getattr(_4, "7")
    _13 = getattr(_4, "8")
    _14 = getattr(_4, "9")
    _15 = getattr(_4, "10")
    _16 = getattr(_4, "11")
    _17 = (_5).forward(xs1, chunk_masks, pos_emb, masks0, CONSTANTS.c0, CONSTANTS.c0, )
    xs2, chunk_masks0, _18, _19, = _17
    _20 = (_6).forward(xs2, chunk_masks0, pos_emb, masks0, CONSTANTS.c0, CONSTANTS.c0, )
    xs3, chunk_masks1, _21, _22, = _20
    _23 = (_7).forward(xs3, chunk_masks1, pos_emb, masks0, CONSTANTS.c0, CONSTANTS.c0, )
    xs4, chunk_masks2, _24, _25, = _23
    _26 = (_8).forward(xs4, chunk_masks2, pos_emb, masks0, CONSTANTS.c0, CONSTANTS.c0, )
    xs5, chunk_masks3, _27, _28, = _26
    _29 = (_9).forward(xs5, chunk_masks3, pos_emb, masks0, CONSTANTS.c0, CONSTANTS.c0, )
    xs6, chunk_masks4, _30, _31, = _29
    _32 = (_10).forward(xs6, chunk_masks4, pos_emb, masks0, CONSTANTS.c0, CONSTANTS.c0, )
    xs7, chunk_masks5, _33, _34, = _32
    _35 = (_11).forward(xs7, chunk_masks5, pos_emb, masks0, CONSTANTS.c0, CONSTANTS.c0, )
    xs8, chunk_masks6, _36, _37, = _35
    _38 = (_12).forward(xs8, chunk_masks6, pos_emb, masks0, CONSTANTS.c0, CONSTANTS.c0, )
    xs9, chunk_masks7, _39, _40, = _38
    _41 = (_13).forward(xs9, chunk_masks7, pos_emb, masks0, CONSTANTS.c0, CONSTANTS.c0, )
    xs10, chunk_masks8, _42, _43, = _41
    _44 = (_14).forward(xs10, chunk_masks8, pos_emb, masks0, CONSTANTS.c0, CONSTANTS.c0, )
    xs11, chunk_masks9, _45, _46, = _44
    _47 = (_15).forward(xs11, chunk_masks9, pos_emb, masks0, CONSTANTS.c0, CONSTANTS.c0, )
    xs12, chunk_masks10, _48, _49, = _47
    _50 = (_16).forward(xs12, chunk_masks10, pos_emb, masks0, CONSTANTS.c0, CONSTANTS.c0, )
    xs13, chunk_masks11, _51, _52, = _50
    if self.normalize_before:
      xs14 = (self.after_norm).forward(xs13, )
    else:
      xs14 = xs13
    return (xs14, masks0)
  def forward_chunk(self: __torch__.wenet.transformer.encoder.ConformerEncoder,
    xs: Tensor,
    offset: int,
    required_cache_size: int,
    att_cache: Tensor=CONSTANTS.c0,
    cnn_cache: Tensor=CONSTANTS.c0) -> Tuple[Tensor, Tensor, Tensor]:
    if torch.eq(torch.size(xs, 0), 1):
      pass
    else:
      ops.prim.RaiseException("AssertionError: ")
    _53 = torch.size(xs, 1)
    _54 = ops.prim.device(xs)
    tmp_masks = torch.ones([1, _53], dtype=11, layout=None, device=_54)
    tmp_masks0 = torch.unsqueeze(tmp_masks, 1)
    xs15 = (self.global_cmvn).forward(xs, )
    _55 = (self.embed).forward(xs15, tmp_masks0, offset, )
    xs16, pos_emb, _56, = _55
    cache_t1 = torch.size(att_cache, 2)
    chunk_size = torch.size(xs16, 1)
    attention_key_size = torch.add(cache_t1, chunk_size)
    pos_emb0 = (self.embed).position_encoding(torch.sub(offset, cache_t1), attention_key_size, )
    if torch.lt(required_cache_size, 0):
      next_cache_start = 0
    else:
      if torch.eq(required_cache_size, 0):
        next_cache_start0 = attention_key_size
      else:
        _57 = torch.sub(attention_key_size, required_cache_size)
        next_cache_start0 = ops.prim.max(_57, 0)
      next_cache_start = next_cache_start0
    _58 = ops.prim.device(xs16)
    masks = torch.ones([0, 0, 0], dtype=11, layout=None, device=_58)
    r_att_cache = annotate(List[Tensor], [])
    r_cnn_cache = annotate(List[Tensor], [])
    _59 = self.encoders
    _60 = getattr(_59, "0")
    _61 = getattr(_59, "1")
    _62 = getattr(_59, "2")
    _63 = getattr(_59, "3")
    _64 = getattr(_59, "4")
    _65 = getattr(_59, "5")
    _66 = getattr(_59, "6")
    _67 = getattr(_59, "7")
    _68 = getattr(_59, "8")
    _69 = getattr(_59, "9")
    _70 = getattr(_59, "10")
    _71 = getattr(_59, "11")
    if torch.gt(cache_t1, 0):
      _72 = torch.slice(att_cache, 0, 0, 1)
    else:
      _72 = att_cache
    if torch.gt(cache_t1, 0):
      _73 = torch.select(cnn_cache, 0, 0)
    else:
      _73 = cnn_cache
    _74 = (_60).forward(xs16, masks, pos_emb0, CONSTANTS.c1, _72, _73, )
    xs17, _75, new_att_cache, new_cnn_cache, = _74
    _76 = torch.slice(torch.slice(new_att_cache), 1)
    _77 = torch.slice(_76, 2, next_cache_start)
    _78 = torch.append(r_att_cache, torch.slice(_77, 3))
    _79 = torch.append(r_cnn_cache, torch.unsqueeze(new_cnn_cache, 0))
    if torch.gt(cache_t1, 0):
      _80 = torch.slice(att_cache, 0, 1, 2)
    else:
      _80 = att_cache
    if torch.gt(cache_t1, 0):
      _81 = torch.select(cnn_cache, 0, 1)
    else:
      _81 = cnn_cache
    _82 = (_61).forward(xs17, masks, pos_emb0, CONSTANTS.c1, _80, _81, )
    xs18, _83, new_att_cache0, new_cnn_cache0, = _82
    _84 = torch.slice(torch.slice(new_att_cache0), 1)
    _85 = torch.slice(_84, 2, next_cache_start)
    _86 = torch.append(r_att_cache, torch.slice(_85, 3))
    _87 = torch.append(r_cnn_cache, torch.unsqueeze(new_cnn_cache0, 0))
    if torch.gt(cache_t1, 0):
      _88 = torch.slice(att_cache, 0, 2, 3)
    else:
      _88 = att_cache
    if torch.gt(cache_t1, 0):
      _89 = torch.select(cnn_cache, 0, 2)
    else:
      _89 = cnn_cache
    _90 = (_62).forward(xs18, masks, pos_emb0, CONSTANTS.c1, _88, _89, )
    xs19, _91, new_att_cache1, new_cnn_cache1, = _90
    _92 = torch.slice(torch.slice(new_att_cache1), 1)
    _93 = torch.slice(_92, 2, next_cache_start)
    _94 = torch.append(r_att_cache, torch.slice(_93, 3))
    _95 = torch.append(r_cnn_cache, torch.unsqueeze(new_cnn_cache1, 0))
    if torch.gt(cache_t1, 0):
      _96 = torch.slice(att_cache, 0, 3, 4)
    else:
      _96 = att_cache
    if torch.gt(cache_t1, 0):
      _97 = torch.select(cnn_cache, 0, 3)
    else:
      _97 = cnn_cache
    _98 = (_63).forward(xs19, masks, pos_emb0, CONSTANTS.c1, _96, _97, )
    xs20, _99, new_att_cache2, new_cnn_cache2, = _98
    _100 = torch.slice(torch.slice(new_att_cache2), 1)
    _101 = torch.slice(_100, 2, next_cache_start)
    _102 = torch.append(r_att_cache, torch.slice(_101, 3))
    _103 = torch.append(r_cnn_cache, torch.unsqueeze(new_cnn_cache2, 0))
    if torch.gt(cache_t1, 0):
      _104 = torch.slice(att_cache, 0, 4, 5)
    else:
      _104 = att_cache
    if torch.gt(cache_t1, 0):
      _105 = torch.select(cnn_cache, 0, 4)
    else:
      _105 = cnn_cache
    _106 = (_64).forward(xs20, masks, pos_emb0, CONSTANTS.c1, _104, _105, )
    xs21, _107, new_att_cache3, new_cnn_cache3, = _106
    _108 = torch.slice(torch.slice(new_att_cache3), 1)
    _109 = torch.slice(_108, 2, next_cache_start)
    _110 = torch.append(r_att_cache, torch.slice(_109, 3))
    _111 = torch.append(r_cnn_cache, torch.unsqueeze(new_cnn_cache3, 0))
    if torch.gt(cache_t1, 0):
      _112 = torch.slice(att_cache, 0, 5, 6)
    else:
      _112 = att_cache
    if torch.gt(cache_t1, 0):
      _113 = torch.select(cnn_cache, 0, 5)
    else:
      _113 = cnn_cache
    _114 = (_65).forward(xs21, masks, pos_emb0, CONSTANTS.c1, _112, _113, )
    xs22, _115, new_att_cache4, new_cnn_cache4, = _114
    _116 = torch.slice(torch.slice(new_att_cache4), 1)
    _117 = torch.slice(_116, 2, next_cache_start)
    _118 = torch.append(r_att_cache, torch.slice(_117, 3))
    _119 = torch.append(r_cnn_cache, torch.unsqueeze(new_cnn_cache4, 0))
    if torch.gt(cache_t1, 0):
      _120 = torch.slice(att_cache, 0, 6, 7)
    else:
      _120 = att_cache
    if torch.gt(cache_t1, 0):
      _121 = torch.select(cnn_cache, 0, 6)
    else:
      _121 = cnn_cache
    _122 = (_66).forward(xs22, masks, pos_emb0, CONSTANTS.c1, _120, _121, )
    xs23, _123, new_att_cache5, new_cnn_cache5, = _122
    _124 = torch.slice(torch.slice(new_att_cache5), 1)
    _125 = torch.slice(_124, 2, next_cache_start)
    _126 = torch.append(r_att_cache, torch.slice(_125, 3))
    _127 = torch.append(r_cnn_cache, torch.unsqueeze(new_cnn_cache5, 0))
    if torch.gt(cache_t1, 0):
      _128 = torch.slice(att_cache, 0, 7, 8)
    else:
      _128 = att_cache
    if torch.gt(cache_t1, 0):
      _129 = torch.select(cnn_cache, 0, 7)
    else:
      _129 = cnn_cache
    _130 = (_67).forward(xs23, masks, pos_emb0, CONSTANTS.c1, _128, _129, )
    xs24, _131, new_att_cache6, new_cnn_cache6, = _130
    _132 = torch.slice(torch.slice(new_att_cache6), 1)
    _133 = torch.slice(_132, 2, next_cache_start)
    _134 = torch.append(r_att_cache, torch.slice(_133, 3))
    _135 = torch.append(r_cnn_cache, torch.unsqueeze(new_cnn_cache6, 0))
    if torch.gt(cache_t1, 0):
      _136 = torch.slice(att_cache, 0, 8, 9)
    else:
      _136 = att_cache
    if torch.gt(cache_t1, 0):
      _137 = torch.select(cnn_cache, 0, 8)
    else:
      _137 = cnn_cache
    _138 = (_68).forward(xs24, masks, pos_emb0, CONSTANTS.c1, _136, _137, )
    xs25, _139, new_att_cache7, new_cnn_cache7, = _138
    _140 = torch.slice(torch.slice(new_att_cache7), 1)
    _141 = torch.slice(_140, 2, next_cache_start)
    _142 = torch.append(r_att_cache, torch.slice(_141, 3))
    _143 = torch.append(r_cnn_cache, torch.unsqueeze(new_cnn_cache7, 0))
    if torch.gt(cache_t1, 0):
      _144 = torch.slice(att_cache, 0, 9, 10)
    else:
      _144 = att_cache
    if torch.gt(cache_t1, 0):
      _145 = torch.select(cnn_cache, 0, 9)
    else:
      _145 = cnn_cache
    _146 = (_69).forward(xs25, masks, pos_emb0, CONSTANTS.c1, _144, _145, )
    xs26, _147, new_att_cache8, new_cnn_cache8, = _146
    _148 = torch.slice(torch.slice(new_att_cache8), 1)
    _149 = torch.slice(_148, 2, next_cache_start)
    _150 = torch.append(r_att_cache, torch.slice(_149, 3))
    _151 = torch.append(r_cnn_cache, torch.unsqueeze(new_cnn_cache8, 0))
    if torch.gt(cache_t1, 0):
      _152 = torch.slice(att_cache, 0, 10, 11)
    else:
      _152 = att_cache
    if torch.gt(cache_t1, 0):
      _153 = torch.select(cnn_cache, 0, 10)
    else:
      _153 = cnn_cache
    _154 = (_70).forward(xs26, masks, pos_emb0, CONSTANTS.c1, _152, _153, )
    xs27, _155, new_att_cache9, new_cnn_cache9, = _154
    _156 = torch.slice(torch.slice(new_att_cache9), 1)
    _157 = torch.slice(_156, 2, next_cache_start)
    _158 = torch.append(r_att_cache, torch.slice(_157, 3))
    _159 = torch.append(r_cnn_cache, torch.unsqueeze(new_cnn_cache9, 0))
    if torch.gt(cache_t1, 0):
      _160 = torch.slice(att_cache, 0, 11, 12)
    else:
      _160 = att_cache
    if torch.gt(cache_t1, 0):
      _161 = torch.select(cnn_cache, 0, 11)
    else:
      _161 = cnn_cache
    _162 = (_71).forward(xs27, masks, pos_emb0, CONSTANTS.c1, _160, _161, )
    xs28, _163, new_att_cache10, new_cnn_cache10, = _162
    _164 = torch.slice(torch.slice(new_att_cache10), 1)
    _165 = torch.slice(_164, 2, next_cache_start)
    _166 = torch.append(r_att_cache, torch.slice(_165, 3))
    _167 = torch.append(r_cnn_cache, torch.unsqueeze(new_cnn_cache10, 0))
    if self.normalize_before:
      xs29 = (self.after_norm).forward(xs28, )
    else:
      xs29 = xs28
    r_att_cache0 = torch.cat(r_att_cache)
    r_cnn_cache0 = torch.cat(r_cnn_cache)
    return (xs29, r_att_cache0, r_cnn_cache0)
