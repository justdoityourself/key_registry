#pragma once

/*
    auto [public_key,private_key] = d8u::pair_crypto::create_keypair(4096);

    public_key.Import(public_key.Export());
    private_key.Import(private_key.Export());

    auto public_str = public_key.Export();

    auto cert = private_key.Sign(public_str);

    bool valid = public_key.Verify(public_str,cert);

    auto enc = public_key.Encrypt(std::string("MESSAGE"));
    auto dec = private_key.Decrypt(enc);
*/